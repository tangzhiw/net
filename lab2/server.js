const express = require('express');
const app = express();
const os = require('os');

// 设置端口号
const port = 3000;

// 获取主机IPv4地址
function getIPv4Address() {
    const networkInterfaces = os.networkInterfaces();
    let ipAddress;

    for (const interfaceName of Object.keys(networkInterfaces)) {
        for (const networkInterface of networkInterfaces[interfaceName]) {
            if (
                !networkInterface.internal &&
                networkInterface.family === 'IPv4' &&
                (interfaceName.includes('WLAN') || interfaceName.includes('ETH'))
            ) {
                ipAddress = networkInterface.address;
                return ipAddress;
            }
        }
    }
}

// 设置静态文件目录
app.use(express.static(__dirname + '/public'));

// 定义路由处理器
app.get('/', (req, res) => {
    res.sendFile(__dirname + '/public/wechat.html');
});

// 启动服务器并输出信息
app.listen(port, '0.0.0.0', () => {
    const ipAddress = getIPv4Address();

    if (ipAddress) {
        console.log(`Server is running on http://localhost:${port}`);
        console.log(`http://${ipAddress}:${port}`);
    } else {
        console.log('Failed to retrieve the IPv4 address.');
    }
});
