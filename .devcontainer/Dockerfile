FROM espressif/idf:release-v4.1

# Install nodejs
RUN apt-get update && apt-get install -y nodejs npm

# Run install and export idf everytime bash gets opend
RUN echo "/opt/esp/idf/install.sh" >> ~/.bashrc && echo ". /opt/esp/idf/export.sh" >> ~/.bashrc
