import httpx
from openai import OpenAI, AsyncOpenAI

import os
import logging
gpt_logger = logging.getLogger("gpt_logger")
formatter = logging.Formatter("%(asctime)s %(levelname)s %(message)s")
handler = logging.FileHandler(os.path.join("./gpt_logger.simple.log"))
handler.setFormatter(formatter)
gpt_logger.setLevel(logging.INFO)
gpt_logger.addHandler(handler)


proxies = {
    # "https://": "http://192.168.200.1:10809",
    # "http://": "http://192.168.200.1:10809",
    "all://": "socks5://192.168.200.1:10808",
}

client = OpenAI(
    api_key="", # sk-6bK3XhYYs8IGsHaFKw3eT3BlbkFJ----1oBVqTjgmQ7QFxP127vJ
    # api_key="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", # llama3
    # base_url="https://api.perplexity.ai", 
    http_client=httpx.Client(proxies=proxies, verify=False, timeout=None), 
)

print("Initialize Finish.")

def test_network():
    return httpx.get("https://openai.com/", proxies=proxies, timeout=10)

def prompt(prompt_file: str="./simple_prompt.txt"):
    return open(prompt_file, 'r').read()

def send_request(prompt: str=prompt(), test_connection: bool=True):
    print(test_network())
    if input("Y/N?\n") not in ["Y", "y"]:
        exit(-1)
    gpt_logger.info(f"Generated requests for\n {prompt}")
    res = client.chat.completions.create(
        # model="llama-3-70b-instruct",
        model="gpt-4-1106-preview",
        # model="gpt-3.5-turbo-1106",
        temperature=0,
        messages=[{"role": "user", "content": prompt}],
        timeout=30, # in seconds
    )
    return res


if __name__ == "__main__":
    with open("simple_gpt_result.txt", "w") as f:
        f.write(send_request())


