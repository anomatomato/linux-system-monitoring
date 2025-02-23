# Setups for debian-package-repo container

## On server

```sh
# create a dir for the repo
sudo mkdir /srv/raspi-package-repo
sudo chown $USER:ibrusers /srv/raspi-package-repo/
chmod g+w /srv/raspi-package-repo/
# install the reprepro config
mkdir /srv/raspi-package-repo/conf
cp debian-package-repo/distributions /srv/raspi-package-repo/conf/
# run the container
docker compose up -d --build debian-package-repo-service
```

## On personal host

```sh
sudo apt install gpg reprepro
gpg --full-gen-key
sshfs sep24-poc:/srv/raspi-package-repo mnt/
# initialize empty repo (optional)
# reprepro --basedir /srv/raspi-package-repo export
# add a package
reprepro --basedir mnt/ includedeb bookworm hostname-from-mac_1.0_all.deb
```
