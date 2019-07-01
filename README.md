# Fight Food Waste - Product scanner

This is the end-user product scanner that integrates with our API.

## Dependencies

- GTK for the GUI
- curl for handling API requests
- janson for handling JSON.

## Installation

On Debian:

```sh
apt install build-essential libgtk-3-dev cmake libjansson-dev libcurl4-gnutls-dev
mkdir build
cd build
cmake ../
make
```

It also require a working deployment of the Fight Food Waste API. (-> `API_ENDPOINT`).

## License

MIT
