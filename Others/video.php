<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
	<title>Razor Live 720P</title>
</head>
 
<body>
 
 <!-- Code for WebCam START -->
	<div id="video-jwplayer_wrapper" style="position: relative; display: block; width: 1280px; height: 720px;margin:auto ">
		<object type="application/x-shockwave-flash" data="/jwplayer/jwplayer.flash.swf" width="100%" height="100%" bgcolor="#000000" id="video-jwplayer" name="video-jwplayer" tabindex="0">
			<param name="allowfullscreen" value="true">
			<param name="allowscriptaccess" value="always">
			<param name="seamlesstabbing" value="true">
			<param name="wmode" value="opaque">
		</object>
	<div id="video-jwplayer_aspect" style="display: none;"></div>
	<div id="video-jwplayer_jwpsrv" style="position: absolute; top: 0px; z-index: 10;"></div>
	</div>
	
	<script src="/jwplayer/jwplayer.js"></script> 
	<script type="text/javascript">
		jwplayer('video-jwplayer').setup({
			flashplayer:"/jwplayer/jwplayer.flash.swf"
			, file:"rtmp://" + window.location.hostname + "/flvplayback/flv:Razor.flv"
			, autoStart: true
			, rtmp:{
				bufferlength:0.1
			}
			, deliveryType: "streaming"
			, width: 1280
			, height: 720
			, player: {
				modes: {
					linear: {
						controls:{
							stream:{
								manage:false
								, enabled: false
							}
						}
					}
				}
			}
			, shows: {
				streamTimer: {
					enabled: true
					, tickRate: 100
				}
			}
		});
	</script> 
 
 <!-- Code for WebCam END --> 
 
<br />
 
	当前CPU温度：
	<?php
		 $temp = intval(file_get_contents("/sys/class/thermal/thermal_zone0/temp")); // read the file as a string[file_get_contents()], and convert to an int [intval()]so we can do math functions with it
		 $color = "#060"; // default colour
		 if($temp >= 60000) { $color = "#F00"; } //set to red if over 60 degrees (is in milliDegrees)
		 elseif($temp >= 50000) { $color = "#FC0"; } // set to yellow if over 50 degrees
		 echo "<span style=\"color:".$color.";\">".($temp/1000)."&deg;C / ".(($temp/1000)+271.15)."K</span>";
	?>
<br />
	当前CPU主频：
	 <?php
		 $freq = intval(file_get_contents("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq"));
		 echo ($freq/1000)."MHz"; // covert to MHz from kHz
	 ?>
<br />
	 运行时间：
	 <?php
		 // Grab uptime output
		 $uptime_array = explode(" ", exec("cat /proc/uptime")); 
		 $seconds = round($uptime_array[0], 0); 
		 $minutes = $seconds / 60; 
		 $hours = $minutes / 60; 
		 $days = floor($hours / 24); 
		 $hours = floor($hours - ($days * 24)); 
		 $minutes = floor($minutes - ($days * 24 * 60) - ($hours * 60)); 
		 $seconds = floor($seconds - ($days * 24 * 60 * 60) - ($hours * 60 * 60) - ($minutes * 60)); 
		 $uptime_array = array($days, $hours, $minutes, $seconds); 
		 $uptime = ($days . " days " . $hours . " hours " . $minutes . " minutes" );
		 echo $uptime; 
	?>
 <br />
	系统时间：
	<?php 
		// Get the Time
		$time = (exec("date +'%d %b %Y - %T %Z'"));
		echo $time; 
	?>
 <br />
 
	<!-- Network-Data: -->
	获取网络数据：
	<?php 
		// Get Network-Data
		$RX = (exec("ifconfig wlan0 | grep 'RX bytes'| cut -d: -f2 | cut -d' ' -f1"));
		$TX = (exec("ifconfig wlan0 | grep 'TX bytes'| cut -d: -f3 | cut -d' ' -f1"));
		echo $RX; $TX; 
	?>
<br />

	<?php
		$ip = $_SERVER['REMOTE_ADDR'];
		$hostaddress = gethostbyaddr($ip);
		$browser = $_SERVER['HTTP_USER_AGENT'];
		$referred = $_SERVER['HTTP_REFERER']; // a quirky spelling mistake that stuck in php

		print "<strong>IP地址：</strong><br />\n";
		print "$ip<br /><br />\n";
		print "<strong>主机地址：</strong><br />\n";
		print "$hostaddress<br /><br />\n";
		print "<strong>浏览器信息：</strong>:<br />\n";
		print "$browser<br /><br />\n";
		print "<strong>Where you came from, if you clicked on a link to get here</strong>:<br />\n";
		if ($referred == "") {
			print "Page was directly requested";
		}
		else {
			print "$referred";
		}
	?>
<br />

</body>
 
</html>