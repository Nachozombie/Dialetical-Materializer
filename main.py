import os
from flask import Flask, url_for, render_template, request
import subprocess
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
import sys
from bs4 import BeautifulSoup
import requests

app = Flask(__name__, static_url_path='')

@app.route("/",methods=['GET', 'POST'])
def hello():
	if request.method == 'POST':
		input = request.form['input']
		ifile = open('input.txt', 'w')
		ifile.write(input)
		ifile.close()
		words = input.split(' ')
		words = list(set(words))
		antonyms = []
		driver = webdriver.PhantomJS()
		driver.set_window_size('1120','550')
		filer = open('antonym.txt', 'w')
		for i in range(len(words)):
			try:
				driver.get('http://thesaurus.com')
				driver.find_element_by_name('q').clear()
				driver.find_element_by_name('q').send_keys(words[i])
				driver.find_element_by_name('q').send_keys(Keys.RETURN)
				sys.stdout.write(driver.current_url)
				main = driver.find_elements_by_class_name('antonyms')[0]
				sys.stdout.write('reached the antonym section part\n')
				antText = main.find_elements_by_xpath('.//div[@class="list-holder"]/ul/li/a/span[@class="text"]')
                                sys.stdout.write('got the span tags\n')
				sys.stdout.write(words[i] + " " + antText[0].text)# + " " + antText[2].text)
				filer.write(words[i]+" "+antText[0].text)#+" "+antText[1].text+" "+antText[2].text)
			except:
				pass
		driver.close()
		filer.close()
		out = open('output.txt', 'r')
		dr = out.read()
		drs = dr.split('\n')
		drs = drs[1:]
		breakpoint = 0
		breakpointtwo = 0
		for lines in range(len(drs)):
			if drs[lines] == "Conflict:":
				breakpointtwo = lines
			if drs[lines] == "DifficultWords:":
				breakpoint = lines
		moods = drs[:breakpointtwo]
		conflicts = drs[breakpointtwo+1:breakpoint]
		dWords = drs[breakpoint+1:]
		definitions = []
		for w in range(len(dWords)):
			url = 'http://www.thesaurus.com/browse/'+dWords[w]+'?s=t'
			response = requests.get(url)
			data = response.content
			soup = BeautifulSoup(data, 'html.parser')
			dab = soup.find('strong', attrs={'class': 'ttl'})
			try:
				dab = dab.text
			except:
				dab = ''
			definitions.append([{'word': dWords[w]}, {'definition': dab}])
		sys.stdout.write('The previous stuff did not cause a crash.')
		subprocess.Popen('./HackCooper/analysisprog', shell=True)
		return render_template('complete.html', definitions=definitions, moods=moods, conflicts=conflicts)
	return render_template('main.html')
if __name__ == "__main__":
	port = int(os.environ.get("PORT", 5000))
	app.run(host='0.0.0.0', port=port)
