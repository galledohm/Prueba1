t <!DOCTYPE html>
t <html>
t <head>
t <style>
t table, th, td {
t    border: 1px solid black;
t    border-collapse: collapse;
t }
t th, td {
t    padding: 5px;
t    text-align: left;
t }
t </style>
t </head>
t <body>
t <table style="width:50%">
t  <h1>CONTROL TEMPERATURA Y HUMEDAD</h1>
t  <tr>
t   <th>Temperatura</th>
t  </tr>
t  <tr>
t    <td>LM35</td>
c t 1 <td>%d C</td>
t  </tr>
t  <tr>
t <td>DS1621</td>
c t 2 <td>%d C</td>
t  </tr> 
t  <tr>
t  	<th>Presión</th>
t   </tr>
t    	<tr>
t    	<td>BMP085</td>
c p 1 <td>%d Bares</td>
t   		</tr>
t   <th>Humedad relativa</th>
t    <tr>
t    <td>HID4000</td>
c h 1 <td>%d %</td>
t  </tr>
t </table>
t </body>
t </html>
.