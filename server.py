import http.server
import socketserver
import subprocess
import json
import os
import urllib.parse

PORT = 8000
SIMULATOR_PATH = "./y86sim"
TEST_DIR = "./test"
ANSWER_DIR = "./answer" 

class Handler(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        if self.path == '/list_files':
            files = [f for f in os.listdir(TEST_DIR) if f.endswith('.yo')]
            files.sort()
            self.send_response(200)
            self.send_header('Content-type', 'application/json')
            self.send_header('Access-Control-Allow-Origin', '*')
            self.end_headers()
            self.wfile.write(json.dumps(files).encode())
            return

        if self.path.startswith('/run'):
            query = urllib.parse.urlparse(self.path).query
            params = urllib.parse.parse_qs(query)
            filename = params.get('file', [None])[0]

            if filename:
                test_path = os.path.join(TEST_DIR, filename)
                ans_filename = filename.replace('.yo', '.json')
                ans_path = os.path.join(ANSWER_DIR, ans_filename)

                if os.path.exists(test_path):
                    try:
                        with open(test_path, 'r', encoding='utf-8') as f:
                            input_data = f.read()

                        process = subprocess.run(
                            [SIMULATOR_PATH], input=input_data,
                            capture_output=True, text=True
                        )
                        raw_output = process.stdout
                        is_correct = "unknown"
                        diff_info = ""

                        if os.path.exists(ans_path):
                            try:
                                start = raw_output.find('[')
                                end = raw_output.rfind(']') + 1
                                my_json = json.loads(raw_output[start:end])
                                
                                with open(ans_path, 'r') as f:
                                    correct_json = json.load(f)
                                def normalize(states):
                                    for s in states:
                                        s['MEM'] = {str(k): v for k, v in s['MEM'].items()}
                                    return states

                                if normalize(my_json) == normalize(correct_json):
                                    is_correct = "pass"
                                else:
                                    is_correct = "fail"
                            except Exception as e:
                                is_correct = "error"
                                diff_info = str(e)
                        response = {
                            "output": raw_output + "\n" + process.stderr,
                            "is_correct": is_correct,
                            "has_answer": os.path.exists(ans_path)
                        }
                        
                        self.send_response(200)
                        self.send_header('Content-type', 'application/json')
                        self.send_header('Access-Control-Allow-Origin', '*')
                        self.end_headers()
                        self.wfile.write(json.dumps(response).encode())

                    except Exception as e:
                        self.send_error(500, str(e))
                return
        super().do_GET()

socketserver.TCPServer.allow_reuse_address = True
with socketserver.TCPServer(("", PORT), Handler) as httpd:
    httpd.serve_forever()