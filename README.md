# ESP32 IOT device example

## this example uses ESP IDF framework 
## must have 4mb flash and 4mb ram. WROOM work and can be purchased for $5 to $10. like ESP-CAM or ESP-DEVKIT
### features:
* mDNS broadcas
* Rest Server back end
* React Front End
* read/write configuration to local NVS storage
* both AP mode and STA mode 
    
use ``` idf.py create-component components/${COMPONENT_NAME} to create a new component``` 
(replace ${COMPONENT_NAME} with the name of component folder)


add ``` PRIV_REQUIRE ${COMPONENT_NAME}``` to the CMakeLists.txt to allow use of other components inside a component
```cmake
idf_component_register(SRCS "rest-server.c"
                    INCLUDE_DIRS "include"
                    REQUIRES json esp_http_server spiffs
                    PRIV_REQUIRES config-manager wifi-driver)

```

to add new data to configuration:
add `<input>` element inside the `<form>` element in HomePage.jsx in the front-end/web/src/pages directory 
give the input a name property. this will be the key. 
the value the user types in will be the value that gets stored. 
example 
```html
 <input name='wifi_password' />
 ```

to create multiple configurations
add new endpoint to rest server
create new handler functions in rest-server.c file
copy and paste get_handler and post_handler that already exist and modify to needs. 
then create new URL for endpoint and register the url + handler function to the server in the 'start-rest-server' function

example 
```cpp
    httpd_uri_t post_wifi_config_uri
        = { .uri = "/api/wifi", .method = HTTP_POST, .handler = post_wifi_config_handler, .user_ctx = rest_context };
    httpd_register_uri_handler(server, &post_uri);
```

in the NVS_WRITE_STRING and NVS_READ_STRING functions, use the endpoint name as the first argument (which is the namespace)
example for making new endpoint to store wifi credentials
rest-server.c
create POST handler function
```cpp
static esp_err_t post_wifi_config_handler(httpd_req_t* req)
{
    //set the configuration name. 
    char* NAMESPACE = "WIFI_CONFIG"

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
    //parse the string buffer into a cJSON object so we can use the data
    cJSON* root = cJSON_Parse(buf);
    //we will iterate through each value in the cJSON object and write it to NVS to store the configuration data
    cJSON* json_item = NULL; //cJSON_ArrayForEach needs an object that i can add data to each iteration. access it inside the loop only
    cJSON_ArrayForEach(json_item, root)
    {
        //get the value and save into variable
        const char* value = json_item->valuestring;
        //get the key and save into variable
        const char* key = json_item->string;
        //write the key and value to the flash storage to save the values in the config. 
        NVS_Write_String(NAMESPACE, key, value);
    }
    //delete the root object from memory. we are done with it
    cJSON_Delete(root);
    //send a response back to the client saying 'all done'
    httpd_resp_sendstr(req, "Post control value successfully");

    //do any action here. this time, we are restarting wifi so it can try to connect with the new credientials. 
    //wifi will try to connect 10 times then start into AP mode (hotspot mode)
    init_sta_mode();
    return ESP_OK;
}

```
inside start_rest_server function:
create url
```cpp
//define a struct and add .method, .handler and .user_context values
    httpd_uri_t post_wifi_config_uri
        = { .uri = "/api/wifi", .method = HTTP_POST, .handler = post_wifi_config_handler, .user_ctx = rest_context };
```
just below above code, also inside start_rest_server function
register URL and Handler 
```cpp
//register uri to server 
httpd_register_uri_handler(server, &post_wifi_config_uri);
```
