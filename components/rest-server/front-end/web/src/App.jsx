import { Route, BrowserRouter as Router, Routes } from "react-router-dom";
import HomePage from './pages/HomePage';
import ConfigureWifiPage from './pages/ConfigureWifiPage/ConfigureWifiPage';

function App() {
  return (
<Router >
  <Routes>
    <Route exact path="/" element={<HomePage/>} />
    <Route path="/wifi" element={<ConfigureWifiPage/>} />
  </Routes>
</Router>
  )
}

export default App
