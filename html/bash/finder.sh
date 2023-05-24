#!/bin/bash
body=""
Login=""
declare -a info

getinfo(){
	info+=($(ldapsearch uid="$Login" 2>&- | grep cn | awk '{print $2}'));
	info+=($(ldapsearch uid="$Login" 2>&- | grep cn | awk '{print $3 }'));
	info+=($(ldapsearch uid="$Login" 2>&- | grep mobile | awk '{print $2}'));
	for index in ${!info[@]}
	do
		if [ ${info[index]} == "" ]
		then
			unset ${info[index]}
		fi
	done
}

url_decode() {
	echo $(python3 -c "import urllib.parse; print(urllib.parse.unquote_plus('$1'));");
	return 0;
}

send_response() {
	echo -e "Content-Type: text/html\\r\\n";
	echo -e "\\r\\n"
	echo '<!DOCTYPE html>'
	echo "
		<html>
			<head>
				<style>
					body {
						background: black;
						display: flex;
						justify-content: center;
						align-items: center;
						height: 100vh;
						font-size: large;
						color: rgb(214, 204, 204);
					}
				</style>
			</head>
			<body>
			<div class=\"info\">
				<h1>Student Info </h1>
				<p>First Name: ${info[0]}</p>
				<p>Second Name : ${info[1]}</p>
				<p>Phone Number: ${info[2]}</p>
			</div>
			<div class=\"research\">
				<a href=\"/search.html\"><button id>search again</button></a>
			</div>
			</body>
		</html>
		"
}

not_found() {
	echo '<!DOCTYPE html>'
	echo "
		<html>
			<head>
			</head>
					<body>
						<h1> Student Not Found</h1>
					</body>
		</html>"
	return 0
}

if [ "$REQUEST_METHOD" == "POST" ]
then
	body=$(cat);
	body=$(url_decode $body);
elif [ "$REQUEST_METHOD" == "GET" ]
then
	body=$(url_decode $QUERY_STRING);
fi

Login=$(echo $body | grep -o '^?*Login=[^&]\+');
if [ "$Login" != "" ]
then
	Login=$(echo $Login | cut -d "=" -f2);
	getinfo $Login;
fi

if [ ${#info[@]} -gt 0 ];
then
	send_response ;
else
	not_found ;
fi