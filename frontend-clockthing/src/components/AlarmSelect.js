import React from 'react';
import {useRef} from "react";

export default function AlarmSelect({setAlarm, name, val}) {
    const timeRef = useRef()
    const sendForm = () => setAlarm(timeRef.current.value)

    return (
        <>
            <li className="list-group-item">
                <div className="row">
                    <div className="col-6">
                        <p className={"mx-1 pt-1 lead"}>{name}</p>
                    </div>
                    <div className="col-6">
                        <input className="form-select" type="time" ref={timeRef} onChange={sendForm} value={val}/>
                    </div>
                </div>
            </li>
        </>
    )
}