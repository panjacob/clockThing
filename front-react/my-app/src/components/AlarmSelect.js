import {useRef, useState} from "react";

export default function AlarmSelect({setAlarm, name, val}) {
    const timeRef = useRef()
    const sendForm = () => setAlarm(timeRef.current.value)

    return (
        <>
            <li className="list-group-item">
                <div className="row">
                    <div className="col-6">
                        <h5 className={"mx-1 pt-1"}>{name}</h5>
                    </div>
                    <div className="col-6">
                        <input className="form-select" type="time" ref={timeRef} onChange={sendForm} value={val}/>
                    </div>
                </div>
            </li>
        </>
    )
}