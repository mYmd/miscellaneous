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
from azure.storage.blob import BlobServiceClient


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
