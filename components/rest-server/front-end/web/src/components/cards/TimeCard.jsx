
import { useState, useEffect } from 'react';
import useFetch from '../../hooks/useFetch';
import Clock from './Clock';
const TimeCard = () => {



    const [fetchedTime, setFetchedTime, getFetchedTime] = useFetch("/api/time")
    const [deviceTime, setDeviceTime] = useState(null)
    useEffect(() => {
        if (!fetchedTime) return
        const date = new Date(fetchedTime.time)
        setDeviceTime({
            minutes: date.getMinutes(),
            hours: date.getHours(),
            seconds: date.getSeconds()
        })
        return () => {
        }
    }, [fetchedTime]
    )


    const [time, setTime] = useState({
        minutes: new Date().getMinutes(),
        hours: new Date().getHours(),
        seconds: new Date().getSeconds()
    })

    useEffect(() => {
        const intervalId = setInterval(() => {
            const date = new Date();
            setTime({
                minutes: date.getMinutes(),
                hours: date.getHours(),
                seconds: date.getSeconds()
            })
        }, 1000)

        return () => clearInterval(intervalId);
    }, [])
    useEffect(() => {
        const intervalId = setInterval(() => {
            getFetchedTime()
        }, 1000)

        return () => clearInterval(intervalId);
    }, [])



    return (
        <div>

            <Clock time={time} />
            <Clock time={deviceTime} />
        </div>
    );
}

export default TimeCard;