
const express = require('express');
const app = express();
const port = 3000;
const os = require('os');
app.use(express.static(__dirname + '/public'));
app.get('/', (req, res) => {
  res.sendFile(__dirname + '/public/wechat.html');
 
});

app.listen(port,  () => {

  console.log(`Server is running on http://localhost:${port}`);
  console.log(`http://10.136.95.107:${port}`);
});
