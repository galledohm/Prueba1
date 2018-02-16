t <!DOCTYPE html>
t <html><head><title>Estación meteo</title></head>
t <body bgColor=#ffffff leftMargin=0 topMargin=10 marginwidth="0" marginheight="0">
t <div align=center style="width: 833; height: 470">
t <table  height=384 cellSpacing=0 cellPadding=0 width="815">
t <tbody>
t <tr bgColor=#ADEFF6>
t  <td style="border-bottom: 1px solid #000080" vAlign=bottom noWrap height=70 margin=50 width="567">
t   <h2 align="center"><font face="verdana" color="#006699">ESTACIÓN METEOROLÓGICA</font>	</h2>
t  </td>
t </tr>
t <tr>
t  <td colSpan=0 height=380 width="815" vAlign=top">
t   <div align=center>
t   <font face="verdana">
t     <table width="60%" border=0 align=center>
t      <tbody><tr>
t      <td width="95%">
t <table border=0 width=600><font size="3">
t <tr>
t 	<th align='left'>TEMPERATURA</th>
t </tr>
t <tr>
t <td>LM35</td>
c t 1 <td>%.1f C</td>
t </tr>
t <tr>
t 	<td>DS1621</td>
c t 2 <td>%.1f C</td>
t </tr>
t <tr>
t 	<td>BMP180</td>
c t 3 <td>%.1f C</td>
t </tr>
t <tr>
t 	<td>Umbral</td>
c t 4 <td>%d C</td>
t </tr>
t <tr>
t 	<th align='left'>HUMEDAD</th>
t </tr>
t <tr>
t 	<td>HIH4000</td>
c h <td>%.1f %%</td>
t </tr>
t  <tr>
t 	<th align='left'>PRESIÓN</th>
t </tr>
t <tr>
t <td>BMP180</td>
c p <td>%.1f hPa</td>
t </tr>
t <tr>
t 	<th align='left'>VELOCIDAD DEL VIENTO</th>
t </tr>
t <tr>
t 	<td>Anemómetro</td>
c v	<td>%.2f m/s</td>
t </tr>
t <tr>
t 	<th align='left'>ALTITUD</th>
t </tr>
t <tr>
t 	<td>BMP180</td>
c a	<td>%.2f m</td>
t </tr>
t </font></table>
t <form method="GET" action="index.cgi">
t <br> Umbral de temperatura:
t <input size="10" value="" name="umbral_t" type="text">
t <br> <input value="Enviar" type="submit">
t </form>
t        </td>
t <td><img src="mono.gif" alt="Mono" style="align="right";width:128px;height:128px;"></td>
t      </tr></tbody></table>
t     </font>
t    </div>
t   </td>
t  </tr></tbody></table>
t </div></body>
t </html>
.