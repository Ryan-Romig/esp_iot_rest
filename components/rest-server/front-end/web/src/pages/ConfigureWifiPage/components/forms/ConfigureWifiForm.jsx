import { useState, useEffect } from 'react';
import { useNavigate } from 'react-router-dom'
const ConfigureWifiForm = () => {
    const navigate = useNavigate()
    const [availableWifiNetworks, setAvailableWifiNetworks] = useState([])
    //define the fetch data function
    const fetchData = async () => {
        //fetch data from the server using the browsers built in fetch api
        try {
            //get the response using the fetch api and await keyword
            //await prevents the code from continuing until the request is complete
            const response = await fetch('api/wifi/scan')
            //throw error if the response is not 200 (ok) REST uses status codes to indicate the status of the request
            //200 is good, 400 is bad, 500 is server error
            if (response.status !== 200) throw new Error("error")
            //if the above line didn't throw an error and jump to the catch clause
            //then we can assume the request was successful and we can get the data from the response
            const data = await response.json()// .json() is a built in function of the reponse object 
            //finally set the react hook variable to the data we got from the server
            console.log(data, response)
            setAvailableWifiNetworks(data.wifi_networks)
        } catch (error) {
            console.log(error)
            setAvailableWifiNetworks(["error fetching data"])
        }
    }
    //function to handle the form submit event
    //the event argument is automatically passed by react when the form is submitted
    //see the onSubmit attribute in the form tag at the bottom of this file
    const handleFormSubmit = async (e) => {

        //prevents page from refreshing when the form is submitted.
        e.preventDefault()

        //create a new form object to store the form data.
        //this prevents us from having to keep track of each input field separately
        const formEntries = new FormData(e.target)

        //convert the form object into a json object string. sending data to a REST server requires it to be in json format
        const jsonObject = Object.fromEntries(formEntries)

        //making it a string allows us to send it in the body of the request
        const json = JSON.stringify(jsonObject)

        //use the browsers build in fetch api to send the data to the server in a POST request to the rest server running on the device
        //the first argument is the url to send the request to, the second argument is the request options
        //the body option is the data we want to send in the request
        // use await keyword prevents the code from continuing until the request is complete
        fetch('api/post', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: json, // replace with your actual data
        })
        e.target.reset()
        navigate('/')


    }

    // use effect is a react hook that runs the code inside it when the component is first rendered
    // the empty array as the second argument makes it only run once
    useEffect(() => {
        fetchData()
    }, [])
    //create style for the form
    const style = {
        display: 'flex',
        flexDirection: 'column',
        alignItems: 'center',
        justifyContent: 'center',
        height: '100vh'
    }
    const [ssid, setSSID] = useState()
    const handleSelectSSID = (e) => {
        const selectedValue = e.target.innerHTML
        setSSID(selectedValue)
    }
    const itemStyle = (itemName) => {
        return {
            backgroundColor: ssid === itemName ? "blue" : "white",
            color: ssid === itemName ? "white" : "black",
            cursor: "pointer"
        }



    }
    const handleResetWifiClick = async () => {
        fetch('api/post', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({wifi_ssid:'', wifi_password:''}), // replace with your actual data
        })
        e.target.reset()
        navigate('/')


    }
//return the jsx to render. this is basically HTML but with some extra features

//form object with input inside. anything you put type=submit inside a form will submit the form when clicked. 
//whatever you put in the return statement has to be html or text. If it's a variable it has to a string variable(or converted to string)
return (
    <>
        <button onClick={handleResetWifiClick}>Reset Wifi</button>
        <form style={style} onSubmit={handleFormSubmit}>
            <h1>Configure Wifi</h1>
            <label>Wifi SSID</label>
            <input type='text' value={ssid} name="wifi_ssid" hidden></input>
            <ul>
                {availableWifiNetworks?.length > 0 ? availableWifiNetworks.map((network) => {
                    return <li style={itemStyle(network)} onClick={handleSelectSSID} key={network}>{network}</li>
                }) : "No networks found"}
            </ul>
            <label>Wifi Password</label>
            <input type='password' name="wifi_password" required></input>
            <button type="submit">Submit</button>
        </form>
    </>
);
}

export default ConfigureWifiForm;