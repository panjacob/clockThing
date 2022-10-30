import React from 'react';
import {Lightbulb, LightbulbFill} from "react-bootstrap-icons";
import {Col, Row} from "react-bootstrap";
import {useEffect, useState} from "react";
import {fetchGETText} from "../utilis";

export default function Power() {
    const [isPower, setPower] = useState(0)
    const [color, setColor] = useState("8")
    const [colorClient, setColorClient] = useState("8")

    const getPowerAndColor = async () => {
        const data = (await fetchGETText("https://clock.panjacob.online/get_settings.php")).split(',')
        setPower(parseInt(data[0]))
        const hexString = "#" + parseInt(data[1]).toString(16)
        setColorClient(hexString)
    }

    const getPowerAndColorFirstTime = async () => {
        const data = (await fetchGETText("https://clock.panjacob.online/get_settings.php")).split(',')
        const hexString = "#" + parseInt(data[1]).toString(16).padStart(6,'0');
        console.log('first time color: ', hexString)
        setColorClient(hexString)
        setColor(hexString)
    }

    useEffect(() => {
        getPowerAndColorFirstTime()
        // getPowerAndColor()
        const comInterval = setInterval(getPowerAndColor, 1000);
        return () => clearInterval(comInterval)
    }, [])

    const changePower = () => {
        const powerBool = !isPower ? 1 : 0
        const colorInt = parseInt(color.replace('#', ''), 16)
        console.log('sending changePower', powerBool, colorInt)
        fetchGETText(`https://clock.panjacob.online/set_settings_client.php?power=${powerBool}&color=${colorInt}`)
    }

    const changeColor = (e) => {
        const powerBool = isPower ? 1 : 0
        let colorInt = parseInt(e.target.value.replace('#', ''), 16)
        if (colorInt === 0) colorInt = 1;
        const colorHex = '#' + colorInt.toString(16)
        setColor(colorHex)
        console.log('sending changeColor', powerBool, colorInt)
        fetchGETText(`https://clock.panjacob.online/set_settings_client.php?power=${powerBool}&color=${colorInt}`)
    }

    return (
        <>
            <Row className={"text-center"}>
                <Col className={""} onClick={changePower}>
                    <h1>
                        {isPower ? <LightbulbFill style={{color: colorClient}}/> : <Lightbulb/>}
                    </h1>
                </Col>
                <Col><input type={"color"} onChange={changeColor} value={color}/></Col>
            </Row>
            <Row className={"text-center mb-3"}>
                <Col><p className={"lead"}>{isPower ? "On" : "Off"}</p></Col>
                <Col><p className={"lead"}>Kolor</p></Col>
            </Row>
        </>
    )
}