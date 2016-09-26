import os
from flask import Flask, url_for, render_template, request
import subprocess
from selenium import webdriver
import requests

app = Flask(__name__, static_url_path='')

@app.route("/")
def hello():
	driver = webdriver.PhantomJS()
	driver.set_window_size('1120','550')
	driver.get('http://thesaurus.com')
	driver.find_element_by_name('q').send_keys('hello')
	return render_template('main.html')
if __name__ == "__main__":
	port = int(os.environ.get("PORT", 5000))
	app.run(host='0.0.0.0', port=port)
