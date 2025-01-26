#py -m pip install azure-storage-file-datalake 
#py -m pip install azure-storage-blob

from azure.storage.filedatalake import (
    DataLakeServiceClient,
    DataLakeDirectoryClient,
    FileSystemClient,
    DataLakeFileClient,
    StorageStreamDownloader
)

from azure.storage.blob import ContainerClient
from azure.storage.blob import BlobClient
from azure.core.exceptions import ClientAuthenticationError
from azure.storage.blob import BlobServiceClient


#https://learn.microsoft.com/en-us/python/api/azure-storage-blob/azure.storage.blob.blobclient?view=azure-python#azure-storage-blob-blobclient-upload-blob
class CsvUploader:
    def __init__(self):
        self.stream_ = io.StringIO(newline='')
        self.csv = csv.writer(self.stream_, dialect=csv.excel_tab)
        self.message = ''
    def clear(self):
        self.stream_.truncate(0)
        self.stream_.seek(0)
        self.message = ''
    def upload(self, full_sas: str, encoding: str='UTF-8', errors: str='strict', clear: bool=True)->bool:
        self.message = ''
        try:
            blob_client = BlobClient.from_blob_url(full_sas)
            exists = blob_client.exists()
            if exists and blob_client.get_blob_properties().blob_type != 'BlockBlob':
                self.message = 'エラー:既存ファイルを上書きuploadをするときBlockタイプは"BlockBlob"でなければなりません'
                return False
            self.stream_.seek(0)
            value = self.stream_.getvalue().encode(encoding, errors)
            datalen = len(value)
            blob_client.upload_blob(value, overwrite=True)
            if clear:
                self.clear()
            m0 = '既存' if exists else '新規'
            m1 = '上書き' if exists else ''
            self.message = f'成功:{m0}ファイルへの{m1}upload({datalen} bytes)'
            return True
        except ClientAuthenticationError as e:
            self.message = 'エラー:例外発生/権限不足もしくは認証エラー'
            return False
        except:
            self.message = 'エラー:例外発生/原因不明'
            return False
        finally:
            self.stream_.seek(0, io.SEEK_END)
    def append(self, full_sas: str, encoding: str='UTF-8', errors: str='strict', clear: bool=True)->bool:
        self.message = ''
        try:
            blob_client = BlobClient.from_blob_url(full_sas)
            exists = blob_client.exists()
            if not exists:
                blob_client.create_append_blob()
            elif blob_client.get_blob_properties().blob_type != 'AppendBlob':
                self.message = 'エラー:既存ファイルにappendするときBlockタイプは"AppendBlob"でなければなりません'
                return False
            self.stream_.seek(0)
            value = self.stream_.getvalue().encode(encoding, errors)
            datalen = len(value)
            if datalen:
                blob_client.append_block(value)
            if clear:
                self.clear()
            self.message = f'成功:{"既存" if exists else "新規"}ファイルへのappend({datalen} bytes)'
            return True
        except ClientAuthenticationError as e:
            self.message = 'エラー:例外発生/権限不足もしくは認証エラー'
            return False
        except:
            self.message = 'エラー:例外発生/原因不明'
            return False
        finally:
            self.stream_.seek(0, io.SEEK_END)


class Strorage_Account:
    def __init__(self, name: str, sas: str = None, account_key: str = None, connection_string: str = None):
        self.name = name
        self.url = f'https://{name}.blob.core.windows.net'
        self.sas = sas
        self.account_key = account_key
        self.connection_string = connection_string


class ADL_Container:
    def __init__(self, account: Strorage_Account, name: str, sas: str = None):
        self.account = account
        self.container = self
        self.name = name
        self.url = f'{account.url}/{name}'
        self.sas = sas or account.sas
        self.sas_token = f'{self.url}?{self.sas}'


class ADL_Blob:
    #upper: ADL_Container|ADL_Blob
    def __init__(self, upper, name: str, sas: str = None):
        self.container = upper.container
        self.account = self.container.account
        self.name = name
        self.url = f'{upper.url}/{name}'
        self.sas = sas or upper.sas
        self.sas_token = f'{self.url}?{self.sas}'


account = Strorage_Account('mymdblob2')
src_container = ADL_Container(account, 'local-backup', sas_c_lbackup)
src_dir = ADL_Blob(src_container, 'Bunsho/IT/OO')
src_blob = ADL_Blob(src_dir, '【GoF】デザインパターン5.txt')
tgt_container = ADL_Container(account, 'projectdata', sas_c_projectdata)
tgt_blob = ADL_Blob(tgt_container, 'target/【GoF】デザインパターン5.txt')
blob_client_target = BlobClient.from_blob_url(tgt_blob.sas_token)
print(blob_client_target.exists())
blob_client_target.start_copy_from_url(src_blob.sas_token)
print(blob_client_target.exists())
return
