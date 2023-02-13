fetch('/raw')
    .then(res => res.json())
    .then(out =>
        setData(out))
    .catch(err => {
        console.log('Error querying json');
        alert('Error getting settings... Please reload');
        throw err;
    });

function setData(out) {
    document.getElementsByName("ssid")[0].value = out.SSID;
    document.getElementsByName("password")[0].value = out.Password;

    document.getElementsByName("udpAddress")[0].value = out.UDPAddress;
    document.getElementsByName("udpPort")[0].value = out.UDPPort;

    document.getElementsByName("gmtoffsetsec")[0].value = out.GMTOffset;
    document.getElementsByName("daylightoffsetsec")[0].value = out.DaylightOffset;
    document.getElementsByName("deeptimeout")[0].value = out.DeepSleepTimeOut;

    document.getElementsByName("lowerbackgpio")[0].value = out.LowerBackGPIO;
    document.getElementsByName("neckgpio")[0].value = out.NeckGPIO;
    document.getElementsByName("wakeupgpio")[0].value = out.WakeUpGPIO;
}