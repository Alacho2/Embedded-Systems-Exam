import React from 'react';

const ACCESS_TOKEN = "d2114b0a2c16c1f86de5477687d81523c949d741";
const DEVICE_ID = "2e0039000b47373336323230";

class Dashboard extends React.Component {

  state = {
    currTemp: 0,
    highestTemp: 0,
    hourAtTemp: 0,
    fireLevel: 0,
    waterLevel: 0,
    hasBeenMoved: 0,
    lightLevel: 0,
    hourAtLight: 0,
    highestLightValue: 0,
  };

  constructor(props){
    super(props);

    // Inital setup of data
    this.fetchDataFromParticle();
  }

  componentDidMount(){
    // Update the data every 30 second
    setInterval(() => {this.fetchDataFromParticle()}, 30000);
  }


  fetchDataFromParticle(){
    console.log("Happening");
    fetch(`https://api.particle.io/v1/devices/${DEVICE_ID}/jsonData?access_token=${ACCESS_TOKEN}`)
      .then(data => data.json())
      .then(res => {
        const value = res.result.replace(/'/g, "\"");
        const json = JSON.parse(value);
        json.hourAtTemp = this.processTimeToCorrectFormat(json.hourAtTemp);
        json.hourAtLight = this.processTimeToCorrectFormat(json.hourAtLight);
        this.setState(json);
      }
    )
  }

  processTimeToCorrectFormat(time){
    let timeAsString = String(time);

    // Depending on the length, we add a :0 or a :
    timeAsString = timeAsString.length === 3 ?
      `${timeAsString.slice(0, 2)}:0${timeAsString.slice(2)}` :
      `${timeAsString.slice(0, 2)}:${timeAsString.slice(2)}`;
    return timeAsString
  }

  processWaterBackgroundColor(level) {
    let backgroundColor = "transparent";
    if (level > 20) {
      backgroundColor = "#006AC6";
    } else if (level > 15) {
      backgroundColor = "#3EA5FF";
    } else if (level > 10) {
      backgroundColor = "#A3D4FE";
    }
    return backgroundColor;
  }

  render() {
    const {
      currTemp,
      highestTemp,
      hourAtTemp,
      fireLevel,
      waterLevel,
      lightLevel,
      hourAtLight,
      hasBeenMoved,
      highestLightValue
    } = this.state;

    const fireBackground = fireLevel > 30 ? "orange" : "transparent";

    return (
      <div className="container" style={{"textAlign": "center"}}>
        <div className="row">
          <div className="col-md-2">
            <h5>Temperature: { currTemp.toFixed(2) }</h5>
            <p></p>
            <h6>Highest today: {highestTemp.toFixed(2)}</h6>
            <p>At: {hourAtTemp}</p>
          </div>
          <div
            className="col-md-2"
            style={{backgroundColor: fireBackground}}>
            <h5>Fire:</h5>
            <h6>{fireLevel}</h6>
          </div>
          <div
            className="col-md-2"
            style={{backgroundColor: this.processWaterBackgroundColor(waterLevel)}}>
            <h5>Water:</h5>
            <h6>{waterLevel} mm</h6>
          </div>
          <div className="col-md-2">

          </div>
          <div className="col-md-2"></div>
          <div className="col-md-2"></div>

        </div>
      </div>
    );
  }
}

export default Dashboard;
