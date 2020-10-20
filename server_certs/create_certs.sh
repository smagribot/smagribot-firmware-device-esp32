#! bin/sh

openssl req -x509 -nodes -days 730 -newkey rsa:2048 -keyout privatekey.pem -out certificate.pem -config req.cnf -sha256