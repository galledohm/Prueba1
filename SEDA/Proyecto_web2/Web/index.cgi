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
t <td>LM35 </td>
c t 1 <td>%.1f C</td>
t  </tr>
t  <tr>
t <td>DS1621</td>
c t 2 <td>%.1f C</td>
t  </tr> 
t  <tr>
t  	<th>Presión</th>
t   </tr>
t    	<tr>
t    	<td>BMP180</td>
c p <td>%.2f hPa</td>
t   		</tr>
t   <th>Humedad relativa</th>
t    <tr>
t    <td>HIH4000</td>
c h <td>%.2f %%</td>
t  </tr>
t </table>
t <form method="GET" action="index.cgi">
t <br> Umbral de temperatura:
t <input size="10" value="" name="umbral_t" type="text">
t <br> <input value="Enviar" type="submit">
t </form>
t </body>
t </html>
.