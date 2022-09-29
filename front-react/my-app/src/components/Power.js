import {Lightbulb, LightbulbFill} from "react-bootstrap-icons";
import {Col, Row} from "react-bootstrap";
import {useEffect, useState} from "react";
import {fetchGETText} from "../utilis";

export default function Power() {
    const [isPower, setPower] = useState(0)
    const [color, setColor] = useState("")

    const readTemp = async () => {
        const data = (await fetchGETText("https://clock.panjacob.online/get_settings.php")).split(',')
        console.log(data)
        setPower(parseInt(data[0]))
        const hexString = "#" + parseInt(data[1]).toString(16);
        console.log(hexString)
        setColor(hexString)
    }

    useEffect(() => {
        readTemp()
        const comInterval = setInterval(readTemp,  1000);
        return () => clearInterval(comInterval)
    }, [])

    const changePower = () => {
        const powerBool = !isPower ? 1 : 0
        fetchGETText(`https://clock.panjacob.online/set_settings_client.php?power=${powerBool}&color=${color}`)
    }

    const changeColor = (e) => {
        const powerBool = isPower ? 1 : 0
        const colorInt = parseInt(e.target.value.replace('#',''), 16)
        console.log(colorInt)
        fetchGETText(`https://clock.panjacob.online/set_settings_client.php?power=${powerBool}&color=${colorInt}`)
    }

    return (
        <>
            <Row className={"text-center"}>
                <Col className={""} onClick={changePower}>
                    <h1>
                        {isPower ? <LightbulbFill className={"text-warning"}/> : <Lightbulb/>}
                    </h1>
                </Col>
                    <Col><input type={"color"} defaultValue={"#e66465"} onChange={changeColor} value={color}/></Col>
            </Row>
            <Row className={"text-center mb-3"}>
                <Col><p className={"lead"}>{isPower ? "On" : "Off"}</p></Col>
                <Col><p className={"lead"}>Kolor</p></Col>
            </Row>
        </>
)
}