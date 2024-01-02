#include "../../components/config-manager/include/config-manager.h"
#include "../../components/wifi-driver/include/wifi-driver.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "sdkconfig.h"
#include <string.h>
#include "esp_log.h"
#include "esp_spiffs.h"
#include "lwip/apps/netbiosns.h"
#include "esp_spiffs.h"
#include "cJSON.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_vfs.h"
#include <fcntl.h>
#include <string.h>
static const char* TAG = "REST SERVER";
#define WEB_STORAGE_MOUNT_POINT "/www"
#define REST_CHECK(a, str, goto_tag, ...)                                                                              \
    do {                                                                                                               \
        if (!(a)) {                                                                                                    \
            ESP_LOGE(TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__);                                      \
            goto goto_tag;                                                                                             \
        }                                                                                                              \
    } while (0)

#define FILE_PATH_MAX (ESP_VFS_PATH_MAX + 128)
#define SCRATCH_BUFSIZE (10240)

typedef struct rest_server_context {
    char base_path[ESP_VFS_PATH_MAX + 1];
    char scratch[SCRATCH_BUFSIZE];
} rest_server_context_t;

#define CHECK_FILE_EXTENSION(filename, ext) (strcasecmp(&filename[strlen(filename) - strlen(ext)], ext) == 0)

esp_err_t init_fs(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = WEB_STORAGE_MOUNT_POINT, .partition_label = NULL, .max_files = 5, .format_if_mount_failed = false
    };
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ESP_FAIL;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
    return ESP_OK;
}

/* Set HTTP response content type according to file extension */
static esp_err_t set_content_type_from_file(httpd_req_t* req, const char* filepath)
{
    const char* type = "text/plain";
    if (CHECK_FILE_EXTENSION(filepath, ".html")) {
        type = "text/html";
    } else if (CHECK_FILE_EXTENSION(filepath, ".js")) {
        type = "application/javascript";
    } else if (CHECK_FILE_EXTENSION(filepath, ".css")) {
        type = "text/css";
    } else if (CHECK_FILE_EXTENSION(filepath, ".png")) {
        type = "image/png";
    } else if (CHECK_FILE_EXTENSION(filepath, ".ico")) {
        type = "image/x-icon";
    } else if (CHECK_FILE_EXTENSION(filepath, ".svg")) {
        type = "text/xml";
    }
    return httpd_resp_set_type(req, type);
}

/* Send HTTP response with the contents of the requested file */
static esp_err_t default_get_handler(httpd_req_t* req)
{
    char filepath[FILE_PATH_MAX];

    rest_server_context_t* rest_context = (rest_server_context_t*)req->user_ctx;
    strlcpy(filepath, rest_context->base_path, sizeof(filepath));
    if (req->uri[strlen(req->uri) - 1] == '/') {
        strlcat(filepath, "/index.html", sizeof(filepath));
    } else {
        strlcat(filepath, req->uri, sizeof(filepath));
    }
    int fd = open(filepath, O_RDONLY, 0);
    if (fd == -1) {
        ESP_LOGE(TAG, "Failed to open file : %s", filepath);
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read existing file");
        return ESP_FAIL;
    }

    set_content_type_from_file(req, filepath);

    char* chunk = rest_context->scratch;
    ssize_t read_bytes;
    do {
        /* Read file in chunks into the scratch buffer */
        read_bytes = read(fd, chunk, SCRATCH_BUFSIZE);
        if (read_bytes == -1) {
            ESP_LOGE(TAG, "Failed to read file : %s", filepath);
        } else if (read_bytes > 0) {
            /* Send the buffer contents as HTTP response chunk */
            if (httpd_resp_send_chunk(req, chunk, read_bytes) != ESP_OK) {
                close(fd);
                ESP_LOGE(TAG, "File sending failed!");
                /* Abort sending file */
                httpd_resp_sendstr_chunk(req, NULL);
                /* Respond with 500 Internal Server Error */
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
                return ESP_FAIL;
            }
        }
    } while (read_bytes > 0);
    /* Close file after sending complete */
    close(fd);
    ESP_LOGI(TAG, "File sending complete");
    /* Respond with an empty chunk to signal HTTP response completion */
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static esp_err_t post_handler(httpd_req_t* req)
{
    int total_len = req->content_len;
    int cur_len = 0;
    char* buf = ((rest_server_context_t*)(req->user_ctx))->scratch;
    int received = 0;
    if (total_len >= SCRATCH_BUFSIZE) {
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    }
    while (cur_len < total_len) {
        received = httpd_req_recv(req, buf + cur_len, total_len);
        if (received <= 0) {
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
            return ESP_FAIL;
        }
        cur_len += received;
    }
    buf[total_len] = '\0';

    printf("%s\n", buf);
    cJSON* root = cJSON_Parse(buf);
    cJSON* json_item = NULL;
    cJSON_ArrayForEach(json_item, root)
    {
        const char* value = json_item->valuestring;
        const char* key = json_item->string;
        cJSON* root = cJSON_Parse(buf);

        if (strcmp(key, "wifi_ssid") == 0 || strcmp(key, "wifi_password") == 0) {
            NVS_Write_String("WIFI", key, value);
            printf('got wifi config');  
        }
        NVS_Write_String("config", key, value);
        printf("Value: %s\n", value);
        printf("Key: %s\n", key);
    }
    cJSON_Delete(root);
    httpd_resp_sendstr(req, "Post control value successfully");
    init_sta_mode();
    return ESP_OK;
}

static esp_err_t get_handler(httpd_req_t* req)
{
    httpd_resp_set_type(req, "application/json");
    char* ssid = NVS_Read_String("config", "wifi_ssid");
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "ssid", ssid);
    const char* sys_info = cJSON_Print(root);
    httpd_resp_sendstr(req, sys_info);
    free((void*)sys_info);
    cJSON_Delete(root);
    return ESP_OK;
}

esp_err_t start_rest_server(const char* base_path);

esp_err_t start_rest_server(const char* base_path)
{
    REST_CHECK(base_path, "wrong base path", err);
    rest_server_context_t* rest_context = calloc(1, sizeof(rest_server_context_t));
    REST_CHECK(rest_context, "No memory for rest context", err);
    strlcpy(rest_context->base_path, base_path, sizeof(rest_context->base_path));

    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;

    ESP_LOGI(TAG, "Starting HTTP Server");
    REST_CHECK(httpd_start(&server, &config) == ESP_OK, "Start server failed", err_start);

    httpd_uri_t get_uri = { .uri = "/api/get", .method = HTTP_GET, .handler = get_handler, .user_ctx = rest_context };
    httpd_register_uri_handler(server, &get_uri);

    httpd_uri_t post_uri
        = { .uri = "/api/post", .method = HTTP_POST, .handler = post_handler, .user_ctx = rest_context };
    httpd_register_uri_handler(server, &post_uri);
//handler for all other requests (including front end files)
    httpd_uri_t default_get_uri
        = { .uri = "/*", .method = HTTP_GET, .handler = default_get_handler, .user_ctx = rest_context };
    httpd_register_uri_handler(server, &default_get_uri);

    return ESP_OK;
err_start:
    free(rest_context);
err:
    return ESP_FAIL;
}

void start_wifi_server(void)
{
    init_fs();
    ESP_ERROR_CHECK(start_rest_server(WEB_STORAGE_MOUNT_POINT));
}
