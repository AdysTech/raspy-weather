<?php
	function human_filesize($bytes, $decimals = 2) {
		$sz = 'BKMGTP';
		$factor = floor((strlen($bytes) - 1) / 3);
		return sprintf("%.{$decimals}f", $bytes / pow(1024, $factor)) . @$sz[$factor] . ($factor>0?"B":"") ;
	}

	function human_time($ss) {
		$s = $ss%60;
		$m = floor(($ss%3600)/60);
		$h = floor(($ss%86400)/3600);
		$d = floor(($ss%2592000)/86400);
		return ($d != 0 ?  (($d == 1) ? " 1 day, " : $d . " days, "):"") .  $h . ":" .  $m . " (hh:mm)";
	}
	
	function humidex($tempC, $DewPoint) {
		$e = 19.833625 - 5417.753 /(273.16 + $DewPoint);
		$h = $tempC + 3.3941 * exp($e) - 5.555;
		return $h;
	}

	$current_time = exec("date +'%d %b %Y %T %Z'");
	$frequency = exec("cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq") / 1000;
	$cpu =  explode(",", exec("cat /proc/cpuinfo | grep -e Processor -e Hardware | paste -sd \",\""));
	$cores = count($cpu);
	if(trim(explode(":",$cpu[count($cpu)-1])[0])== "Hardware")
	{
		$processor =trim(explode(":",$cpu[count($cpu)-1])[1]);
		$cores--;
	}
	
	$processor =$processor . str_replace("-compatible processor", "",explode(":",$cpu[1])[1]) . "x" . $cores ;
	
	$cpu_temperature = round(exec("cat /sys/class/thermal/thermal_zone0/temp ") / 1000, 1);
	list($system, $host, $kernel) = split(" ", exec("uname -a"), 4);
	$host = exec('hostname -f');;

	//Uptime
	$uptime_array = explode(" ", exec("cat /proc/uptime"));
	$uptime = human_time (round($uptime_array[0], 0));
	
	// Load averages
	$loadavg = explode(" ", exec("cat /proc/loadavg"));
	$loadaverages  = $loadavg [0] . "(1m) " . $loadavg [1] . "(5m) " . $loadavg [2] . "(15m)";
	
//Memory Utilisation

	$meminfo = file("/proc/meminfo");
	for ($i = 0; $i < count($meminfo); $i++)
	{
		list($item, $data) = split(":", $meminfo[$i], 2);
		$item = trim(chop($item));
		$data = intval(preg_replace("/[^0-9]/", "", trim(chop($data)))); //Remove non numeric characters
		switch($item)
		{
			case "MemTotal": $total_mem = $data; break;
			case "MemFree": $free_mem = $data; break;
			case "SwapTotal": $total_swap = $data; break;
			case "SwapFree": $free_swap = $data; break;
			case "Buffers": $buffer_mem = $data; break;
			case "Cached": $cache_mem = $data; break;
			default: break;
		}
	}
	$used_mem = $total_mem - $free_mem;
	$used_swap = $total_swap - $free_swap;
	$percent_free = round(($free_mem / $total_mem) * 100);
	$percent_used = round(($used_mem / $total_mem) * 100);
	$percent_swap = round((($total_swap - $free_swap ) / $total_swap) * 100);
	$percent_swap_free = round(($free_swap / $total_swap) * 100);
	$percent_buff = round(($buffer_mem / $total_mem) * 100);
	$percent_cach = round(($cache_mem / $total_mem) * 100);
	$used_mem = human_filesize($used_mem*1024,0);
	$used_swap = human_filesize($used_swap*1024,0);
	$total_mem = human_filesize($total_mem*1024,0);
	$free_mem = human_filesize($free_mem*1024,0);
	$total_swap = human_filesize($total_swap*1024,0);
	$free_swap = human_filesize($free_swap*1024,0);
	$buffer_mem = human_filesize($buffer_mem*1024,0);
	$cache_mem = human_filesize($cache_mem*1024,0);	
	
	$dht11 = explode(":",exec("rrdtool lastupdate /etc/dht11/dht11.rrd | paste -sd \":\""));
	//for($i=0;$i<count($dht11);$i++) 
	//	echo $dht11[$i]."\n";
	if (count($dht11==4))
	{
		if(isset( $dht11[2]))
		{
			$timestamp = new DateTime();
			$timestamp->setTimestamp($dht11[2]);			
		}
		if(isset( $dht11[3]))
		{
			$dht11_val = explode(" ",$dht11[3]);
			$current_temp = $dht11_val[1];		
			$current_humidity = $dht11_val[2];
			$dew_point = $dht11_val[3];
			$humidex = humidex($current_temp,$dew_point);
		}
	}
?>


<!DOCTYPE html>
 <html>
 <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, user-scalable=no" />
    <title>Raspberry Pi @ Adys Tech</title>
    <link rel="icon" type="image/png" href="/favicon.png" />
	<link rel="stylesheet" href="./style.css" />	
</head>
<body onload="Javascript: updateDisplay();">
	<div id="Content">
		<h1> System Information for RaspberryPi at Ady's Tech </h1>
		
		<table>
				<tr>
					<td colspan="4" class="head center">General Info</td>
				</tr>
				<tr>
					<td colspan="2">Hostname</td>
					<td colspan="2" id="host"></td>
				</tr>
				<tr>
					<td colspan="2">System Time</td>
					<td colspan="2" id="time"></td>
				</tr>
				<tr>
					<td colspan="2">Kernel</td>
					<td colspan="2" id="kernel"></td>
				</tr>
				<tr>
					<td colspan="2">Processor</td>
					<td colspan="2" id="processor"></td>
				</tr>
				<tr>
					<td colspan="2">CPU Frequency</td>
					<td colspan="2" id="freq"></td>
				</tr>
				<tr>
					<td colspan="2">Load Average</td>
					<td colspan="2" id="loadavg"></td>
				</tr>
				<tr>
					<td colspan="2">CPU Temperature</td>
					<td colspan="2" id="cpu_temperature"></td>
				</tr>
				<tr>
					<td colspan="2">Uptime</td>
					<td colspan="2" id="uptime"></td>
				</tr>
				<tr>
					<td colspan="4" class="darkbackground">&nbsp;</td>
				</tr>
				<tr>
					<td colspan="2" class="head right">Memory:</td>
					<td colspan="2" class="head" id="total_mem"><?php echo $total_mem;?></td>
				</tr>
				<tr>
					<td class="column1">Used</td>
					<td class="right" id="used_mem"></td>
					<td class="column3"><div id="bar1">&nbsp;</div></td>
					<td class="right column4" id="percent_used"></td>
				</tr>
				<tr>
					<td>Free</td>
					<td class="right" id="free_mem"></td>
					<td><div id="bar2"></div></td>
					<td class="right" id="percent_free"></td>
				</tr>
				<tr>
					<td>Buffered</td>
					<td class="right" id="buffer_mem"></td>
					<td><div id="bar3"></div></td>
					<td class="right" id="percent_buff"></td>
				</tr>
				<tr>
					<td>Cached</td>
					<td class="right" id="cache_mem"></td>
					<td><div id="bar4"></div></td>
					<td class="right" id="percent_cach"></td>
				</tr>
				<tr>
					<td colspan="4" class="darkbackground">&nbsp;</td>
				</tr>
				<tr>
					<td colspan="2" class="head right">Swap:</td>
					<td colspan="2" class="head" id="total_swap"></td>
				</tr>
				<tr>
					<td>Used</td>
					<td class="right" id="used_swap"></td>
					<td><div id="bar5"></div></td>
					<td class="right" id="percent_swap"></td>
				</tr>
				<tr>
					<td>Free</td>
					<td class="right" id="free_swap"></td>
					<td><div id="bar6"></div></td>
					<td class="right" id="percent_swap_free"></td>
				</tr>
				<tr>
					<td colspan="4" class="darkbackground">&nbsp;</td>
				</tr>
				
				<tr>
					<td colspan="4" class="head center">Environment</td
				</tr>
				<tr>
					<td colspan="3">Room Temperature (literally in my room!)</td>
					<td colspan="1" id="temp"><?php echo isset($current_temp)?  sprintf("%2d °C", $current_temp) :"";?></td>
				</tr>
				<tr>
					<td colspan="3">Humidity (RH)</td>
					<td colspan="1" id="humidity"><?php echo isset($current_humidity) ? sprintf("%2d%%", $current_humidity):"";?></td>
				</tr>				
				<tr>
					<td colspan="3">Dew Point</td>
					<td colspan="1" id="dewpoint"><?php echo isset($dew_point) ? sprintf("%2d °C", $dew_point):"";?></td>
				</tr>				
				<tr>
					<td colspan="3"><a href="http://en.wikipedia.org/wiki/Humidex">Humidex</a> (aka Feels Like)</td>
					<td colspan="1" id="humidex"><?php echo isset($current_humidity) ? sprintf("%2d °C", $humidex):"";?></td>
				</tr>				
				<tr>
					<td colspan="2">Reading taken at</td>
					<td colspan="2" id="timestamp"><?php echo isset($timestamp)?  $timestamp->format('Y-m-d H:i:s T(P)'):"";?></td>
				</tr>
			</table>			
		</div>
	</body>
	<script type="text/javascript">
		function updateText(objectId, text)
		{
			document.getElementById(objectId).textContent = text;
		}
		function updateHTML(objectId, html)
		{
			document.getElementById(objectId).innerHTML = html;
		}
	
		function updateDisplay()
		{
			<?php
				echo "updateText(\"host\",\"$host\");";
				echo "updateHTML(\"time\",\"$current_time\");";
				echo "updateText(\"kernel\",\"$system\" + \" \" + \"$kernel\");";
				echo "updateText(\"processor\",\"$processor\");";
				echo "updateText(\"freq\",\"$frequency\" + \"MHz\");";
				echo "updateText(\"loadavg\",\"$loadaverages\");";
				echo "updateHTML(\"cpu_temperature\",\"$cpu_temperature\" + \"&#x2103;\");";
				echo "updateText(\"uptime\",\"$uptime\");";
				echo "updateText(\"uptime\",\"$uptime\");";
				
				echo "updateText(\"total_mem\",\"$total_mem\" );";
				echo "updateText(\"used_mem\",\"$used_mem\" );";
				echo "updateText(\"percent_used\",\"$percent_used%\");";
				echo "updateText(\"free_mem\",\"$free_mem\" );";
				echo "updateText(\"percent_free\",\"$percent_free%\");";
				echo "updateText(\"buffer_mem\",\"$buffer_mem\" );";
				echo "updateText(\"percent_buff\",\"$percent_buff%\");";
				echo "updateText(\"cache_mem\",\"$cache_mem\" );";
				echo "updateText(\"percent_cach\",\"$percent_cach%\");";
				echo "updateText(\"total_swap\",\"$total_swap\" );";
				echo "updateText(\"used_swap\",\"$used_swap\" );";
				echo "updateText(\"percent_swap\",\"$percent_swap%\");";
				echo "updateText(\"free_swap\",\"$free_swap\" );";
				echo "updateText(\"percent_swap_free\",\"$percent_swap_free%\");\n";
			?>
			document.getElementById("bar1").style.width = "<?php echo $percent_used; ?>px";
			document.getElementById("bar2").style.width = "<?php echo $percent_free; ?>px";
			document.getElementById("bar3").style.width = "<?php echo $percent_buff; ?>px";
			document.getElementById("bar4").style.width = "<?php echo $percent_cach; ?>px";
			document.getElementById("bar5").style.width = "<?php echo $percent_swap; ?>px";
			document.getElementById("bar6").style.width = "<?php echo $percent_swap_free; ?>px";
		}		

	</script> 
</html>

