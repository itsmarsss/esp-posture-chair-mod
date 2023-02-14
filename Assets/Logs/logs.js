fetch('/rawlogs')
    .then(res => res.json())
    .then(out =>
        setLogs(out))
    .catch(err => {
        console.log('Error querying json');
        alert('Error getting logs... Please reload');
        throw err;
    });
files = document.getElementById('files');
function setLogs(out) {
    logs = out.LOGS;
    logs.forEach((log) => {
        files.innerHTML +=
            "File: <a href=\"./getfile?filename=" + log + "\">" +
            log +
            "</a> | <a href=\"./removefile?filename=" + log + "\">" +
            "<button class=\"delete\">Delete</button></a><br>";
    });
}