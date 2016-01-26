#!/bin/bash
TIME=0.85
TIME2=3
TIME3=5
while true
do
	#mosquitto_pub -h localhost -t "lights" -m "W0001-100"
	#mosquitto_pub -h localhost -t "lights" -m "R0254"
	#sleep $TIME
	# echo "MQTT Test - W0001-100%"
 #    mosquitto_pub -h localhost -t "lights" -m "W0001-100"
	#sleep $TIME

	# echo "Lendo Corrente de 100%"
	# mosquitto_pub -h localhost -t "lights" -m "R0001"
	# sleep $TIME
	# mosquitto_pub -h localhost -t "lights" -m "R0005"
	# sleep $TIME2




  mosquitto_pub -h localhost -t "lights/009E805E" -m "W0001-100"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E81A3" -m "W0001-100"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E7FC1" -m "W0001-100"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E801F" -m "W0001-100"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E8073" -m "W0001-100"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E7F4D" -m "W0001-100"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/000AF62A" -m "W0001-100"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E8054" -m "W0001-100"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E8032" -m "W0001-100"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E80DF" -m "W0001-100"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E8100" -m "W0001-100"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E8010" -m "W0001-100"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/0006E34D" -m "W0001-100"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E7DC4" -m "W0001-100"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E7F4F" -m "W0001-100"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E8216" -m "W0001-100"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E7DEC" -m "W0001-100"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E81D8" -m "W0001-100"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E812B" -m "W0001-100"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E818F" -m "W0001-100"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E81D8" -m "W0001-100"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E7F27" -m "W0001-100"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E7FE4" -m "W0001-100"
	sleep $TIME

	mosquitto_pub -h localhost -t "lights/009E805E" -m "W0001-000"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E81A3" -m "W0001-000"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E7FC1" -m "W0001-000"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E801F" -m "W0001-000"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E8073" -m "W0001-000"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E7F4D" -m "W0001-000"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/000AF62A" -m "W0001-000"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E8054" -m "W0001-000"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E8032" -m "W0001-000"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E80DF" -m "W0001-000"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E8100" -m "W0001-000"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E8010" -m "W0001-000"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/0006E34D" -m "W0001-000"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E7DC4" -m "W0001-000"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E7F4F" -m "W0001-000"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E8216" -m "W0001-000"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E7DEC" -m "W0001-000"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E81D8" -m "W0001-000"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E812B" -m "W0001-000"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E818F" -m "W0001-000"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E81D8" -m "W0001-000"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E7F27" -m "W0001-000"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights/009E7FE4" -m "W0001-000"
	sleep $TIME


	mosquitto_pub -h localhost -t "lights" -m "R0001"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights" -m "R0004"
	sleep $TIME2
	mosquitto_pub -h localhost -t "lights" -m "R0255"
	sleep $TIME
	mosquitto_pub -h localhost -t "lights" -m "R0005"
	sleep $TIME2
done
