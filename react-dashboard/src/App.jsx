import React from 'react';
import Dashboard from "./components/Dashboard.jsx";

class App extends React.Component {

  render(){
    return(
      <div style={{"marginTop": "50px"}}>
        <h1 style={{"textAlign": "center"}}>Plant Protector</h1>
        <Dashboard />
      </div>
    )
  }
}

export default App;
