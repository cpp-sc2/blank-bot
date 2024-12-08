from os import path, environ
from typing import Union

import requests
import yaml
#from loguru import logger
import os
from datetime import datetime
import shutil

#CONFIG_FILE: str = "config.yml"
MY_BOT_NAME: str = "PrimordialOrigin"
ZIPFILE_NAME: str = "bot.zip"

TOKEN: str = "0887d0d31ba8ed9de040f430f3018c5ec5314a62"#environ.get(API_TOKEN_ENV)
BOT_ID: str = "809"
URL: str = f"https://aiarena.net/api/bots/{BOT_ID}/"


def get_bot_description() -> str:
    return (
        f"# Primordial Origin" "\nMade from scratch in C++"
    )


def retrieve_value_from_config(string: str) -> Union[str, bool, None]:
    __user_config_location__: str = path.abspath(".")
    user_config_path: str = path.join(__user_config_location__, CONFIG_FILE)
    # attempt to get race and bot name from config file if they exist
    if path.isfile(user_config_path):
        with open(user_config_path) as config_file:
            config: dict = yaml.safe_load(config_file)
            if string in config:
                return config[string]


if __name__ == "__main__":

    # datetime object containing current date and time
    now = datetime.now()
    print("now =", now)

    # dd/mm/YY H:M:S
    dt_string = now.strftime("%d_%m_%Y-%H_%M_%S")

    newpath = r'builds/Primor_' + dt_string
    if not os.path.exists(newpath):
        os.makedirs(newpath)
        os.makedirs(newpath + "/data")
        os.makedirs(newpath + "/potato")

    shutil.copyfile("build/bin/BlankBot", newpath + "/PrimordialOrigin")

    shutil.make_archive(newpath, 'zip', newpath)
    
    ZIPFILE_NAME = newpath + ".zip"
    
    print("Uploading bot")
    with open(ZIPFILE_NAME, "rb") as bot_zip:
        request_headers = {
            "Authorization": f"Token {TOKEN}",
        }
        request_data = {
            "bot_zip_publicly_downloadable": True,
            "bot_data_publicly_downloadable": False,
            "bot_data_enabled": False,
            "wiki_article_content": get_bot_description(),
        }
        request_files = {
            "bot_zip": bot_zip,
        }
        print(URL)
        response = requests.patch(
            URL, headers=request_headers, data=request_data, files=request_files
        )
        print(response)
        print(response.content)
        