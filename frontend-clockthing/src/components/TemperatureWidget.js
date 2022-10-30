import React  from 'react';
import {Droplet, ThermometerHalf} from 'react-bootstrap-icons';
import {Col, Row} from "react-bootstrap";
import {useEffect, useState} from "react";
import {fetchGETText} from "../utilis";

export default function TemperatureWidget() {
    const [temperature, setTemperature] = useState(0)
    const [humidity, setHumidity] = useState(0)

    const readTemp = async () => {
        const data = await fetchGETText("https://clock.panjacob.online/get_temp.php")
        const dataString = data.split('\n').map(x => x.split(','))
        const lastData = dataString[dataString.length - 2]
        const temp = parseInt(lastData[1])
        const hum = parseInt(lastData[2])
        setTemperature(temp)
        setHumidity(hum)
    }

    useEffect(() => {
        readTemp()
        const comInterval = setInterval(readTemp, 5 * 1000);
        return () => clearInterval(comInterval)
    }, [])


    return (
        <>
            <Row className={"text-center"}>
                <Col className={""}>
                    <h1><ThermometerHalf/></h1>
                    <p className={"lead"}>{temperature}°C</p>
                </Col>
                <Col>
                    <h1><Droplet/></h1>
                    <p className={"lead"}>{humidity}%</p>
                </Col>
            </Row>
        </>
    )
}