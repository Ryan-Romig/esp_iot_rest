import { Link } from "react-router-dom";
function HomePage() {
const style = {
        display: 'flex',
        flexDirection: 'column',
        justifyContent: 'start',
        height: '100vh'
    }
    return (
        <div style={style}>
            <Link to="/wifi">Configure Wifi</Link>
        </div>
    );
}
export default HomePage;
