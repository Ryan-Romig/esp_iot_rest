const Clock = ({ time }) => {
    const convertToTwoDigit = (number) => {
        return number.toLocaleString('en-US', {
            minimumIntegerDigits: 2
        })
    }
    return (

        <div className='clock'>
            {time ? <> <span>{convertToTwoDigit(time.hours)}:</span>
                <span>{convertToTwoDigit(time.minutes)}:</span>
                <span>{convertToTwoDigit(time.seconds)}</span>
                <span>{time.hours >= 12 ? ' PM' : ' AM'}</span></> : <span>loading...</span>
            }
        </div>
    )
}
export default Clock