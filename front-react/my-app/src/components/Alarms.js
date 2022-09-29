import AlarmSelect from "./AlarmSelect";
import {Card, ListGroup} from "react-bootstrap";
import {useEffect, useState} from "react";
import {fetchGET, fetchGETText} from "../utilis";

export default function Alarms() {
    const [alarm1, setAlarm1] = useState("")
    const [alarm2, setAlarm2] = useState("")
    const [alarm3, setAlarm3] = useState("")
    const [alarm4, setAlarm4] = useState("")
    const [alarm5, setAlarm5] = useState("")
    const [alarm6, setAlarm6] = useState("")
    const [alarm7, setAlarm7] = useState("")

    const save = () => {
        let datatimeX = new Date().toLocaleString()
        datatimeX = datatimeX.replaceAll('.', '-').replaceAll(',', '')
        let alarmsForm = [datatimeX, alarm1, alarm2, alarm3, alarm4, alarm5, alarm6, alarm7]
        alarmsForm = alarmsForm.map(x => (x === "") ? "99:99" : x)
        fetchGETText(`https://clock.panjacob.online/set_alarms.php?data=${alarmsForm}`)
    }

    useEffect(() => {
        fetchGETText(`https://clock.panjacob.online/get_alarms.php`).then(x => {
            const alarms = x.split(',')

            setAlarm1(alarms[1])
            setAlarm2(alarms[2])
            setAlarm3(alarms[3])
            setAlarm4(alarms[4])
            setAlarm5(alarms[5])
            setAlarm6(alarms[6])
            setAlarm7(alarms[7])
        })
    }, [])

    return (
        <>
            <h1 className={"text-center mb-3 mt-5"}>Alarmy</h1>
            <Card>
                <ListGroup variant="flush">
                    <AlarmSelect name={"Poniedziałek"} setAlarm={setAlarm1} val={alarm1}/>
                    <AlarmSelect name={"Wtorek"} setAlarm={setAlarm2} val={alarm2}/>
                    <AlarmSelect name={"Środa"} setAlarm={setAlarm3} val={alarm3}/>
                    <AlarmSelect name={"Czwartek"} setAlarm={setAlarm4} val={alarm4}/>
                    <AlarmSelect name={"Piątek"} setAlarm={setAlarm5} val={alarm5}/>
                    <AlarmSelect name={"Sobota"} setAlarm={setAlarm6} val={alarm6}/>
                    <AlarmSelect name={"Niedziela"} setAlarm={setAlarm7} val={alarm7}/>

                    <li className="list-group-item text-center">
                        <button className="btn btn-primary w-75" onClick={save}>Zapisz</button>
                    </li>
                </ListGroup>
            </Card>
        </>
    )
}