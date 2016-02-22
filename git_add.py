import os
import sys


def git_add(url,comment):
	
	os.system("git init")
	os.system("git add .")
	string = "git commit -m " + comment
	os.system(string)
	string = "git remote add origin https://github.com/sumanthm1019/"+url
	os.system(string)
	
	
	
if __name__ == "__main__":
	git_add(sys.argv[1], sys.argv[2])