import smtplib
from email.mime.text import MIMEText



def SendEmail(sendAcc,gmailAppPwd, recvAcc, sTitle, sBody):
    #session
    s = smtplib.SMTP('smtp.gmail.com', 587)
    s.starttls()
    #s.login('alphablash@irumnet.com', gmailAppPwd)#'mcgatkkjqhvmntjf')
    s.login(sendAcc, gmailAppPwd)
    msg = MIMEText(sBody)
    msg['Subject'] = sTitle
    s.sendmail(sendAcc, recvAcc, msg.as_string())
    s.quit()
    return 0


