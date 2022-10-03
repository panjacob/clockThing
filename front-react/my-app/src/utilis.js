const fetchProperties = {
    headers: {
        'Content-Type': 'application/json',
        'Accept': 'application/json'
    }
}

export function fetchGET(url) {
    return fetch(url, fetchProperties)
        .then(response => {
            if (response.status !== 200) throw 'ERROR'
            return response.json()
        })
}

export function fetchGETText(url) {
    return fetch(url, fetchProperties)
        .then(response => {
            if (response.status !== 200) throw 'ERROR'
            return response.text()
        }).then(text => {
            return text
        })
}

export function zeroPad(num, places) {
    return String(num).padStart(places, '0')
}