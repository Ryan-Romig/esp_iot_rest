import { NavLink as Link } from "react-router-dom";
import TimeCard from "../../components/cards/TimeCard";
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
            <TimeCard />
        </div>
    );
}
export default HomePage;
