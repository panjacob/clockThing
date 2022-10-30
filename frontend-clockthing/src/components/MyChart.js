import React from 'react';
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
// const data = {
//     labels: ["Red", "Blue", "Green", "Yellow", "Purple"],
//     datasets: [{
//         data: [1, 2, 3, 4, 5],
//         backgroundColor: ["red", "blue", "yellow", "orange"]
//     }]
// };
// x = [1, 2, 3, 4, 5]

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

const labels = ['January', 'February', 'March', 'April', 'May', 'June', 'July'];

export const data = {
    labels,
    datasets: [
        {
            label: 'Temperatura - C',
            data: [1, 2, 3, 4, 5],
            borderColor: 'rgb(255, 99, 132)',
            backgroundColor: 'rgba(255, 99, 132, 0.5)',
        },
    ],
};

export default function MyChart() {
    const readTemp = async () => {
        const data = await fetchGETText("https://clock.panjacob.online/get_temp.php")
        const dataString = data.split('\n').map(x => x.split(','))
        const temperatures = dataString.map(x => {
            const number = parseFloat(x[1])
            if (isNaN(number)) return 0.0
            else return number
        })
        // const dates = dataString.map(x => {
        //     let dateString = x[0]
        //     if (dateString.length > 3)
        //         dateString = dateString.slice(0, 3) + "bar" + dataString.slice(3);
        //     // const date = Date.parse(date)
        //     // console.log(dateString)
        //     return dateString
        // })
        // console.log("abc")
        
    }

    useEffect(() => {
        readTemp()
        const comInterval = setInterval(readTemp, 10000);
        return () => clearInterval(comInterval)
    }, [])

    return (
        <>
            <h1>Wykresy</h1>
            <div style={{ width: "500px", margin: "0, auto" }}>
                <Line options={options} data={data} />
            </div>
        </>
    )
}