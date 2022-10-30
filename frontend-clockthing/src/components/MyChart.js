import React, { useRef } from 'react';
import { fetchGETText } from "../utilis";
import { useEffect, useState } from "react";
import {
    Chart as ChartJS,
    CategoryScale,
    LinearScale,
    PointElement,
    LineElement,
    Title,
    Tooltip,
    Legend,
    ArcElement,
} from 'chart.js';
import { Line } from 'react-chartjs-2';
import { Col, Row } from 'react-bootstrap';

ChartJS.register(
    CategoryScale,
    LinearScale,
    PointElement,
    LineElement,
    Title,
    Tooltip,
    Legend,
    ArcElement
);


export const options = {
    responsive: true,
    plugins: {
        // legend: {
        //     position: "'top' as const",
        // },
        // title: {
        //     display: true,
        //     text: 'Temperatura w pokoju',
        // },
    },
};


const generateNewData = (data = [], labels = []) => {
    return {
        labels,
        datasets: [
            {
                label: 'Temperatura - C',
                data: data,
                borderColor: 'rgb(255, 99, 132)',
                backgroundColor: 'rgba(255, 99, 132, 0.5)',
            },
        ],
    };
}

const currentDateFromTo = () => {
    const date = new Date();
    const day = date.getDate()
    const month = date.getMonth() + 1
    const year = date.getFullYear()
    return `${year}-${month}-${day}`
}



export default function MyChart() {
    const [dataChart, setDATA] = useState(generateNewData())
    const dateFrom = useRef(currentDateFromTo() + "T00:00")
    const dateTo = useRef(currentDateFromTo() + "T23:59")

    const readTemp = async (from, to) => {
        const data = await fetchGETText(`https://clock.panjacob.online/get_temp.php?from=${from}&to=${to}`)
        // const data = await fetchGETText(`http://localhost:8000/get_temp.php?from=${from}&to=${to}`)
        console.log(`https://clock.panjacob.online/get_temp.php?from=${from}&to=${to}`)
        const dataJson = JSON.parse(data)
        const dates = []
        const temps = []
        const humids = []
        for (const e of dataJson) {
            dates.push(e[0])
            temps.push(parseFloat(e[1]))
            humids.push(parseFloat(e[2]))
        }
        dataChart.datasets.data = temps
        const newDataChart = generateNewData(temps, dates)
        setDATA(newDataChart)
    }

    const updateChart = () => {
        if (dateFrom.current.value === "" || dateTo.current.value === "") return;
        const from = dateFrom.current.value.replace('T', ' ')
        const to = dateTo.current.value.replace('T', ' ')
        console.log(dateTo.current.value)
        readTemp(from, to)

        console.log("updating chart!!", dateFrom.current.value)
    }

    useEffect(() => {
        // const from = currentDateFromTo() + " 00:00"
        // const to = currentDateFromTo() + " 23:59"
        // console.log(currentDateFromTo())
        // readTemp(from, to)
        updateChart()
    }, [])

    return (
        <>
            <h1 className={"text-center"}>Wykresy</h1>
            <Row>
                <Col>
                    <input ref={dateFrom} type="datetime-local" onChange={updateChart} className={"form-control"}
                        defaultValue={currentDateFromTo() + "T00:00"} />
                </Col>
                <Col>
                    <input ref={dateTo} type="datetime-local" onChange={updateChart} className={"form-control"}
                        defaultValue={currentDateFromTo() + "T23:59"} />
                </Col>
            </Row>
            <div className="mx-auto" style={{ maxWidth: "800px" }}>
                <Line options={options} data={dataChart} />
            </div>
        </>
    )
}