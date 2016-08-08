#include "mainwindow.h"
#include "ui_mainwindow.h"

////
/// \brief MainWindow::certReq
/// \return a int number
/// 证书请求生成函数说明：
///     ret为x509等openssl提供函数的返回值
///     该返回值为1即表示函数执行成功，为0则表示失败
///     函数生成的请求为PEM格式
/// 其他部分：
///     1.attribute：此段注释为添加证书请求信息函数
///         具体参数及使用在注释的描述示例中给出
///     2.DEM编码：程序可以生成DEM编码内容
///         具体操作在注释中给出，生成文件后缀名建议改为.pem
///     3.关于公钥和私钥：应该还有用户口令验证函数，不想写
///
int MainWindow::certReq()
{
    certInfo certinfo;
    ret = X509_REQ_set_version(req, version);
    if (!ret)
    {
        printf("sign err at X509_REQ_set_version!\n");
        X509_REQ_free(req);
        return -1;
    }
    //开始读取用户输入内容并写入请求entry，同时向certInfo结构体赋值
    name = X509_NAME_new();
    strcpy(bytes,ui->lineEdit_4->text().toStdString().c_str());
    certinfo.common = bytes;
    len=strlen(bytes);
    entry = X509_NAME_ENTRY_create_by_txt(&entry, "commonName", V_ASN1_UTF8STRING, (unsigned char *)bytes, len);
    X509_NAME_add_entry(name, entry, 0, -1);
    strcpy(bytes,ui->lineEdit_5->text().toStdString().c_str());
    certinfo.country = bytes;
    len = strlen(bytes);
    entry = X509_NAME_ENTRY_create_by_txt(&entry, "countryName", V_ASN1_UTF8STRING, (unsigned char *)bytes, len);
    X509_NAME_add_entry(name, entry, 1, -1);
    strcpy(bytes,ui->lineEdit->text().toStdString().c_str());
    certinfo.locality = bytes;
    len = strlen(bytes);
    entry = X509_NAME_ENTRY_create_by_txt(&entry, "localityName", V_ASN1_UTF8STRING, (unsigned char *)bytes, len);
    X509_NAME_add_entry(name, entry, 1, -1);
    strcpy(bytes,ui->lineEdit_3->text().toStdString().c_str());
    certinfo.province = bytes;
    len = strlen(bytes);
    entry = X509_NAME_ENTRY_create_by_txt(&entry, "stateOrProvinceName", V_ASN1_UTF8STRING, (unsigned char *)bytes, len);
    X509_NAME_add_entry(name, entry, 1, -1);
    strcpy(bytes,ui->lineEdit_2->text().toStdString().c_str());
    certinfo.organization = bytes;
    len = strlen(bytes);
    entry = X509_NAME_ENTRY_create_by_txt(&entry, "organizationName", V_ASN1_UTF8STRING, (unsigned char *)bytes, len);
    X509_NAME_add_entry(name, entry, 1, -1);
    strcpy(bytes,ui->lineEdit_7->text().toStdString().c_str());
    certinfo.ogUnit = bytes;
    len = strlen(bytes);
    entry = X509_NAME_ENTRY_create_by_txt(&entry, "organizationalUnitName", V_ASN1_UTF8STRING, (unsigned char *)bytes, len);
    X509_NAME_add_entry(name, entry, 1, -1);
    strcpy(bytes,ui->lineEdit_6->text().toStdString().c_str());
    certinfo.emailAddr = bytes;
    len = strlen(bytes);
    entry = X509_NAME_ENTRY_create_by_txt(&entry, "emailAddress", V_ASN1_IA5STRING, (unsigned char *)bytes, len);
    X509_NAME_add_entry(name, entry, 1, -1);
    /* subject name */
    ret = X509_REQ_set_subject_name(req, name);
    if (!ret)
    {
        printf("sign err at X509_REQ_set_subject_name!\n");
        X509_REQ_free(req);
        return -1;
    }
    /* pub key */
    pkey = EVP_PKEY_new();
    rsa = RSA_generate_key(bits, e, NULL, NULL);
    EVP_PKEY_assign_RSA(pkey, rsa);
    ret = X509_REQ_set_pubkey(req, pkey);
    if (!ret)
    {
        printf("sign err at X509_REQ_set_pubkey!\n");
        X509_REQ_free(req);
        return -1;
    }
    /* attribute */
    /*添加描述示例*/
//    strcpy_s(bytes, "test");
//    len = strlen(bytes);
//    ret = X509_REQ_add1_attr_by_txt(req, "organizationName", V_ASN1_UTF8STRING, (unsigned char *)bytes, len);
    /************/
    md = EVP_sha1();
    ret = X509_REQ_digest(req, md, (unsigned char *)mdout, (unsigned int*)&mdlen);
    if(!ret)
    {
        printf("sign err at X509_REQ_digest!\n");
        X509_REQ_free(req);
        return -1;
    }
    ret = X509_REQ_sign(req, pkey, md);
    if (!ret)
    {
        printf("sign err at X509_REQ_sign!\n");
        X509_REQ_free(req);
        return -1;
    }
    /* 写入文件 PEM 格式 */
    char name1[100]; //生成的请求文件名称
    //路径变量'dir'写死在上层目录reqfile中，根据自己的需要修改
    QString c_filename = ui->lineEdit_8->text();
    if(c_filename.isEmpty())
    {
        ui->textBrowser->append(getTime() + "请填写请求文件名称");
        return 0;
    }
    strcpy(name1,(dir + c_filename +".csr").toStdString().c_str());
    b = BIO_new_file(name1, "w");
    PEM_write_bio_X509_REQ(b, req);
    BIO_free(b);
//    /* DER 编码 */
//    len = i2d_X509_REQ(req, NULL);
//    der = (unsigned char *)malloc(len);
//    p = der;
//    len = i2d_X509_REQ(req, &p);
//    OpenSSL_add_all_algorithms();
//    ret = X509_REQ_verify(req, pkey);
//    if (ret<0)
//    {
//        printf("verify err.\n");
//    }
//    fp = fopen("certreq2.txt", "wb");
//    fwrite(der, 1, len, fp);
//    fclose(fp);
//    free(der);
//    X509_REQ_free(req);

    //将结果输入到信息显示栏中
    ui->textBrowser->append(getTime() + "证书请求内容填写完成");
    ui->textBrowser->append(setAline() + "用户：" + certinfo.common);
    ui->textBrowser->append(setAline() + "国家：" + certinfo.country);
    ui->textBrowser->append(setAline() + "地区：" + certinfo.locality);
    ui->textBrowser->append(setAline() + "省份：" + certinfo.province);
    ui->textBrowser->append(setAline() + "公司：" + certinfo.organization);
    ui->textBrowser->append(setAline() + "部门：" + certinfo.ogUnit);
    ui->textBrowser->append(setAline() + "邮箱：" + certinfo.emailAddr);
    return 1;
}
