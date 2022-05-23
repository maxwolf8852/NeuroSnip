from transformers import AutoTokenizer, AutoModelForSeq2SeqLM
import time
import grpc
import grpc_serv.serv_g_pb2_grpc as pb2_grpc
import grpc_serv.serv_g_pb2 as pb2
from concurrent import futures


class ServerHandler(pb2_grpc.ServG):
    def __init__(self, *args, **kwargs):
        self.tokenizer: AutoTokenizer
        self.model: AutoModelForSeq2SeqLM

        self.init_translator()

    def init_translator(self):
        self.tokenizer = AutoTokenizer.from_pretrained("Helsinki-NLP/opus-mt-en-ru")
        self.model = AutoModelForSeq2SeqLM.from_pretrained("Helsinki-NLP/opus-mt-en-ru")

    def translate(self, text: str):
        if self.tokenizer is None or self.model is None:
            raise RuntimeError("translator is None")

        start = time.time()
        input_ids = self.tokenizer.encode(text, return_tensors="pt")
        outputs = self.model.generate(input_ids)
        decoded = self.tokenizer.decode(outputs[0], skip_special_tokens=True)
        end = time.time() - start
        print(decoded, end)
        return True, decoded, end

    def GetTranslate(self, request, target):
        message = request.text
        is_exit = request.exit
        if is_exit:
            exit(1)
        print("me", message)
        result = {'success': 0}
        try:
            _, decoded, time_el = self.translate(message)
        except RuntimeError:
            return pb2.TranslatedString(**result)

        result = {'success': 1, 'translated': decoded, 'time': int(time_el * 1000000)}

        return pb2.TranslatedString(**result)


def serve():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=4))
    pb2_grpc.add_ServGServicer_to_server(ServerHandler(), server)
    server.add_insecure_port('[::]:50051')
    server.start()
    print('Server started')
    server.wait_for_termination()


def test_server():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=4))
    pb2_grpc.add_ServGServicer_to_server(ServerHandler(), server)
    server.add_insecure_port('[::]:50051')
    server.start()
    server.stop(3).wait()


if __name__ == '__main__':
    serve()
