#! /bin/sh

# Install UV locally if it isn't installed already
if ! command -v uv >/dev/null 2>&1; then
    echo "uv is not installed. (see https://docs.astral.sh/uv/)"
    read -p "Do you want to install it? (y/n) " answer
    if [ "$answer" = "y" ]; then
        echo "Installing uv"
	if command -v curl >/dev/null 2>&1; then
	    echo "Using curl to fetch install script."
	    curl -LsSf https://astral.sh/uv/install.sh | sh
	elif command -v wget >/dev/null 2>&1; then
	    echo "Using wget to fetch install script."
	    wget -qO- https://astral.sh/uv/install.sh | sh
	else
	    exit 1
	fi
    else
	exit 1
    fi
else
    echo "uv is installed."
fi

