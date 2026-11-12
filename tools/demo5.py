import argparse
import json
import os
from pathlib import Path
import subprocess
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from threading import Thread

ROOT = Path(__file__).resolve().parents[1]
APP = ROOT / ('depgraph.exe' if os.name == 'nt' else 'depgraph')
PACKAGES = ('itoa', 'csv-core', 'csv')


def index_path(name):
    if len(name) < 3:
        prefix = str(len(name))
    elif len(name) == 3:
        prefix = '3/' + name[0]
    else:
        prefix = name[:2] + '/' + name[2:4]
    return '/' + prefix + '/' + name


def main(compare):
    records = json.loads((ROOT / 'examples/snapshot.json').read_text())
    paths = {index_path(name): row for name, row in records.items()}

    class Handler(BaseHTTPRequestHandler):
        def do_GET(self):
            if self.path not in paths:
                self.send_error(404)
                return
            body = (json.dumps(paths[self.path]) + '\n').encode()
            self.send_response(200)
            self.send_header('Content-Length', str(len(body)))
            self.end_headers()
            self.wfile.write(body)

        def log_message(self, *args):
            pass

    folder = ROOT / 'build/demo'
    folder.mkdir(parents=True, exist_ok=True)
    env = dict(os.environ, no_proxy='127.0.0.1', NO_PROXY='127.0.0.1')
    with ThreadingHTTPServer(('127.0.0.1', 0), Handler) as server:
        worker = Thread(target=server.serve_forever, daemon=True)
        worker.start()
        url = 'http://127.0.0.1:' + str(server.server_port)
        try:
            for name in PACKAGES:
                svg = folder / (name + '.svg')
                command = [str(APP), '--package', name, '--repo', url,
                           '--out', str(svg), '--ascii']
                result = subprocess.run(command, cwd=ROOT, env=env,
                                        check=False, capture_output=True,
                                        text=True, encoding='utf-8')
                if result.returncode:
                    raise SystemExit(result.stderr or 'Analyzer failed.')
                print(result.stdout, end='')
                (folder / (name + '.txt')).write_text(result.stdout)
                if compare:
                    command = ['cargo', 'tree', '--locked', '-e', 'normal',
                               '-p', name, '--target',
                               'x86_64-unknown-linux-gnu', '--manifest-path',
                               'examples/reference/Cargo.toml']
                    result = subprocess.run(command, cwd=ROOT, check=True,
                                            capture_output=True, text=True,
                                            encoding='utf-8')
                    print(result.stdout, end='')
                    (folder / (name + '.cargo.txt')).write_text(
                        result.stdout, encoding='utf-8')
        finally:
            server.shutdown()
            worker.join()
    print('Generated files:', folder)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--compare', action='store_true')
    main(parser.parse_args().compare)
