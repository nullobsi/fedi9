#include <u.h>
#include <libc.h>
#include <stdio.h>
#include <json.h>

#define BUF_SIZE 128

char *
sread(int fd, long *size);

int
printStatus(JSON *status);


int
printTimeline(JSON *timeline) {
	//if (timeline->t != JSONObject) return 0;
	JSONEl *element = timeline->first;
	while (element != NULL) {
		printStatus(element->val);
		element = element->next;
	}
	return 1;
}

int
printStatus(JSON *status) {
	//if (status->t != JSONObject) return 0;
	char *created = jsonbyname(status, "created_at")->s;
	char *content = jsonbyname(status, "content")->s;
	char *name = jsonbyname(jsonbyname(status, "account"), "acct")->s;
	
	
	if (created == NULL || content == NULL || name == NULL) return 0;
	print("<p>[%s] @%s:</p> %s<br><br>", created, name, content);
	return 1;
}

int
main(int argc, char **argv)
{
	JSONfmtinstall();
	long inSize;
	char *in = sread(0, &inSize);
	if (in == NULL) {
		exits("Death");
	}
	JSON *parsed = jsonparse(in);
	
	printTimeline(parsed);
	return 0;
}


char *
sread(int fd, long *size) {
	char *buf = (char *)malloc(sizeof(char)*BUF_SIZE);
	long totalRead = 0;
	long buffers = 1;

	if (buf == NULL) {
		printf("Could not malloc");
		return NULL;
	}

	while (1) {
		char *offset = buf + totalRead;
		long numToRead = (BUF_SIZE*buffers) - totalRead;
	//	print("total: %ld toread: %ld buffers: %ld ptr: %p off: %p\n", totalRead, numToRead, buffers, buf, offset);
		long r = readn(fd, offset, numToRead * sizeof(char));
		totalRead += r;
		
		if (r == 0) break;

		if (totalRead == (BUF_SIZE*buffers)) {
			char *nbuf = (char *)realloc(buf, sizeof(char) * (totalRead + BUF_SIZE));
			if (nbuf == NULL) {
				print("Could not realloc loop");
				free(buf);
				return NULL;
			}
			buffers++;
			buf = nbuf;
		}
	}
	if (totalRead == 0) {
		print("Read nothing");
		free(buf);
		return NULL;
	}
	totalRead ++;
	if (totalRead != (BUF_SIZE * buffers)) {
		char *nbuf = (char *)realloc(buf, (sizeof(char) * totalRead) + 1);
		if (nbuf == NULL) {
			print ("Could not realloc next");
			free(buf);
			return NULL;
		}
		buf = nbuf;
	}
	*(buf + totalRead) = '\0';
	*size = totalRead;
	return buf;
}