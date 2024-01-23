import re

Import ("env")

with open("include/netconfig.hpp") as fin:
    for line in fin:
        if 'ota_password' in line:
            password = line.split('"')[1]
            env.Append(UPLOAD_FLAGS=['--port=8266', f'--auth={password}'])
        elif 'default_ip_address' in line:
            addr = '.'.join(re.findall(r'\d+', line))
            env.Append(UPLOAD_PORT=addr)
