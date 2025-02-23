# Proof of Concepts for the Server

Test server is an Ubuntu VM: `sep24-poc.ibr.cs.tu-bs.de`

Most important rule: Stick to defaults

## Setup

### Install Docker

* Official way: <https://docs.docker.com/engine/install/ubuntu/>
* Frank already adds docker repos to apt sources when he creates VMs; we can
  directly install Docker

  ```sh
  sudo apt install docker-ce docker-ce-cli docker-compose
  ```

* Add your user to docker group in case you don't want to type sudo for each
  docker command

  ```sh
  sudo usermod -aG docker $USER
  ```

### Clone this repo to e.g. `/opt`

```sh
sudo mkdir /opt/server-pocs
sudo chmod o+w /opt/server-pocs
git clone git@gitlab.ibr.cs.tu-bs.de:cm-courses/sep/orga/ss24/server-pocs.git /opt/server-pocs
```

### Server setups for Debian package repo

See [debian-package-repo/README.md](debian-package-repo/README.md)

### Initial run

* initially, we must create some access tokens for InfluxDB

  ```sh
  # run InfluxDB only
  INFLUXDB_PASSWORD="admin password" docker-compose up -d influxdb-service
  # create a write token for telegraf
  docker-compose exec influxdb-service influx auth create --org my-org --write-buckets
  # put the token into `telegraf.env`
  echo INFLUXDB_TOKEN="paste your token here" > telegraf.env
  # run all containers
  docker-compose up -d
  ```

* configure Grafana via it's web ui using a read-only token

  ```sh
  docker-compose exec influxdb-service influx auth create --org my-org --write-buckets
  ```

### Normal runs

```sh
docker-compose up -d
```

### Useful commands

```sh
docker exec -it mosquitto bash
docker container rm mosquitto
docker start mosquitto
docker container ls
docker-compose down
docker logs influxdb2
docker-compose stop
docker compose up -d --build debian-package-repo-service
```

## Containers

### InfluxDB

* <https://hub.docker.com/_/influxdb>
* Initial config can be set via env vars; InfluxDB auto-generates a config file
  from this
* InfluxDB is available at <http://sep24-poc:8086> with credential
  admin:$INFLUXDB_PASSWORD

### Mosquitto

* <https://hub.docker.com/_/eclipse-mosquitto>
* Mosquitto's default config file has comments only. Therefore we can start with
  an empty file for a custom config.

* use `mosquitto-clients` for testing

  ```sh
  sudo apt install mosquitto-clients
  mosquitto_sub -h sep24-poc -t '#'
  mosquitto_pub -h sep24-poc -t linux-monitoring -m "cpu ticks=1000 $(date +%s%N)"
  ```

### Telegraf

* <https://hub.docker.com/_/telegraf>
* Telegraf let's us create a starting config which we can then customize:

  ```sh
  docker run --rm telegraf telegraf --input-filter mqtt_consumer \
    --output-filter influxdb_v2 config > telegraf.conf
  ```

* Telegraf requires an access token for authentication with InfluxDB. Create one
  and put it into a file named `telegraf.env`.

### Grafana

* <https://hub.docker.com/r/grafana/grafana>
* It seems possible to configure everything via files
  (<https://grafana.com/docs/grafana/latest/administration/provisioning/>), but
  this looks pretty time consuming and might not be worth it compared to
  configuration via web ui.
* Grafana is available at <http://sep24-poc:3000> with default credentials
  admin:admin

### systemd-journal-remote

* <https://hub.docker.com/_/debian>
* use `systemd-journal-upload` for testing

  ```sh
  /lib/systemd/systemd-journal-upload -u http://sep24-poc
  ```

## TODO

* Finalize Grafana config
* Finalize systemd-journal-remote container
