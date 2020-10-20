# Smagribot Firmware for ESP32
TODO
# Develop

## devcontainer
See [Developing inside a Container for VS Code](https://code.visualstudio.com/docs/remote/containers) for how to use devcontainer configuration.

Docker container uses [IDF Docker Image](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/tools/idf-docker-image.html) for building as basis.
VS Code in Docker is configured to use [Espressif IDF](https://marketplace.visualstudio.com/items?itemName=espressif.esp-idf-extension)-Extension.

For now this solution can't flash an esp32 via serial on MacOS and Windows, see [https://github.com/docker/for-win/issues/1018](https://github.com/docker/for-win/issues/1018) and [https://github.com/docker/for-mac/issues/900](https://github.com/docker/for-mac/issues/900).

## OTA Test Server
Testserver runs on port `8070`, which is forwarded.

VS Code is configured to run server.js via node.

devcontainer Dockerfile is configured to install newst version of node.

Or just run
```bash
node testserver/server.js
```

### Certificate
Use `server_certs/create_certs.sh` to generate new private key and public certificate.

*Don't use this server and certifcates for production!*

# Further reading
- [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
- [ESP Azure IoT SDK](https://docs.microsoft.com/en-us/samples/azure-samples/esp-samples/esp-samples/)