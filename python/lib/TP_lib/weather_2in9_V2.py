# This little program is for the Waveshare 2.9
# inch Version 2 black and white only epaper display
# It uses OpenWeatherMap API to display weather info
import sys
import os
picdir = os.path.join(os.path.dirname(os.path.realpath(__file__)), '../../pic/2in9')
icondir = os.path.join(picdir, 'icon')
fontdir = os.path.join(os.path.dirname(os.path.realpath(__file__)), '../../pic')

# Search lib folder for display driver modules
sys.path.append('lib')
from . import epd2in9_V2
epd = epd2in9_V2.EPD_2IN9_V2()

from datetime import datetime
import time
from PIL import Image,ImageDraw,ImageFont
import traceback

import requests, json
from io import BytesIO
import csv

# define funciton for writing image and sleeping for 5 min.
def write_to_screen(image, sleep_seconds):
    print('Writing to screen.')
    # Write to screen
    h_image = Image.new('1', (epd.height, epd.width), 255)
    # Open the template
    screen_output_file = Image.open(os.path.join(picdir, image))
    # Initialize the drawing context with template as background
    h_image.paste(screen_output_file, (0, 0))
    epd.display(epd.getbuffer(h_image))
    # Sleep
    print('Sleeping for ' + str(sleep_seconds) +'.')
    time.sleep(sleep_seconds)

# define function for displaying error
def display_error(error_source):
    # Display an error
    print('Error in the', error_source, 'request.')
    # Initialize drawing
    error_image = Image.new('1', (epd.height, epd.width), 255)
    # Initialize the drawing
    draw = ImageDraw.Draw(error_image)
    draw.text((5, 5), error_source +' ERROR', font=font20, fill=black)
    draw.text((5, 30), 'Retrying in 30 seconds', font=font20, fill=black)
    current_time = datetime.now().strftime('%H:%M')
    draw.text((5, 55), 'Last Refresh: ' + str(current_time), font = font20, fill=black)
    # Save the error image
    error_image_file = 'error.png'
    error_image.save(os.path.join(picdir, error_image_file))
    # Close error image
    error_image.close()
    # Write error to screen 
    write_to_screen(error_image_file, 30)

# Set the fonts
font12 = ImageFont.truetype(os.path.join(fontdir, 'Font.ttc'), 12)
font16 = ImageFont.truetype(os.path.join(fontdir, 'Font.ttc'), 16)
font20 = ImageFont.truetype(os.path.join(fontdir, 'Font.ttc'), 20)
font24 = ImageFont.truetype(os.path.join(fontdir, 'Font.ttc'), 24)
font30 = ImageFont.truetype(os.path.join(fontdir, 'Font.ttc'), 30)
font35 = ImageFont.truetype(os.path.join(fontdir, 'Font.ttc'), 35)
font50 = ImageFont.truetype(os.path.join(fontdir, 'Font.ttc'), 50)
font60 = ImageFont.truetype(os.path.join(fontdir, 'Font.ttc'), 60)
font100 = ImageFont.truetype(os.path.join(fontdir, 'Font.ttc'), 100)
font160 = ImageFont.truetype(os.path.join(fontdir, 'Font.ttc'), 160)

# Set the special fonts
font18_Roboto_Bold = ImageFont.truetype(os.path.join(fontdir, 'Roboto-Bold.ttf'), 18)
font20_Roboto_Bold = ImageFont.truetype(os.path.join(fontdir, 'Roboto-Bold.ttf'), 20)
font20_Roboto_Regular = ImageFont.truetype(os.path.join(fontdir, 'Roboto-Regular.ttf'), 20)
font34_Roboto_Black = ImageFont.truetype(os.path.join(fontdir, 'Roboto-Black.ttf'), 34)

# Set the colors
black = 'rgb(0,0,0)'
white = 'rgb(255,255,255)'
grey = 'rgb(235,235,235)'

# Initialize and clear screen
# print('Initializing and clearing screen.')
# epd.init()
# epd.Clear()

# I provide my API_KEY for you to test: 82ec63bb0530e31e0ef9042786d195cc, Please try to use your own API_KEY
API_KEY = '******API KEY*******'
LOCATION = 'Shenzhen'
LATITUDE = '22.543097'
LONGITUDE = '114.057861'
UNITS = 'imperial'
CSV_OPTION = False # if csv_option == True, a weather data will be appended to 'record.csv'

BASE_URL = 'http://api.openweathermap.org/data/2.5/onecall?' 
URL = BASE_URL + 'lat=' + LATITUDE + '&lon=' + LONGITUDE + '&units=' + UNITS +'&appid=' + API_KEY

def get_weather_png():
    # Ensure there are no errors with connection
    error_connect = True
    while error_connect == True:
        try:
            # HTTP request
            print('Attempting to connect to OWM.')
            response = requests.get(URL)
            print('Connection to OWM successful.')
            error_connect = None
        except:
            # Call function to display connection error
            print('Connection error.')
            display_error('CONNECTION')
    
    error = None
    while error == None:
        # Check status of code request
        if response.status_code == 200:
            print('Connection to Open Weather successful.')
            # get data in jason format
            data = response.json()
            
            # get current dict block
            current = data['current']
            # get current
            temp_current = current['temp']
            # get feels like
            feels_like = current['feels_like']
            # get humidity
            humidity = current['humidity']
            # get pressure
            wind = current['wind_speed']
            # get description
            weather = current['weather']
            report = weather[0]['description']
            # get icon url
            icon_code = weather[0]['icon']
            #icon_URL = 'http://openweathermap.org/img/wn/'+ icon_code +'@4x.png'
            
            # get daily dict block
            daily = data['daily']
            # get daily precip
            daily_precip_float = daily[0]['pop']
            #format daily precip
            daily_precip_percent = daily_precip_float * 100
            # get min and max temp
            daily_temp = daily[0]['temp']
            temp_max = daily_temp['max']
            temp_min = daily_temp['min']
            
            # Append weather data to CSV if csv_option == True
            if CSV_OPTION == True:
                # Get current year, month, date, and time
                current_year = datetime.now().strftime('%Y')
                current_month = datetime.now().strftime('%m')
                current_date = datetime.now().strftime('%d')
                current_time = datetime.now().strftime('%H:%M')
                #open the CSV and append weather data
                with open('records.csv', 'a', newline='') as csv_file:
                    writer = csv.writer(csv_file, delimiter=',')
                    writer.writerow([current_year, current_month, current_date, current_time,
                                     LOCATION,temp_current, feels_like, temp_max, temp_min,
                                     humidity, daily_precip_float, wind])
                print('Weather data appended to CSV.')
            
            # Set strings to be printed to screen
            string_location = 'City: ' + LOCATION
            string_temp_current = format(temp_current, '.0f') + u'\N{DEGREE SIGN}F'
            string_temp_current_C = format((temp_current-32)/1.8, '.0f') + u'\N{DEGREE SIGN}C'
            string_temp = string_temp_current + ' / ' + string_temp_current_C
            string_feels_like = 'Feels like: ' + format(feels_like, '.0f') +  u'\N{DEGREE SIGN}F'
            string_humidity = 'Humidity: ' + str(humidity) + '%'
            string_wind = 'Wind: ' + format(wind, '.1f') + ' MPH'
            # string_report = 'Now: ' + report.title()
            string_report = report.title()
            string_temp_max = 'High: ' + format(temp_max, '>.0f') + u'\N{DEGREE SIGN}F'
            string_temp_min = 'Low:  ' + format(temp_min, '>.0f') + u'\N{DEGREE SIGN}F'
            string_precip_percent = 'Precip: ' + str(format(daily_precip_percent, '.0f'))  + '%'
            
            # Set error code to false
            error = False
            
            '''
            print('Location:', LOCATION)
            print('Temperature:', format(temp_current, '.0f'), u'\N{DEGREE SIGN}F') 
            print('Feels Like:', format(feels_like, '.0f'), 'F') 
            print('Humidity:', humidity)
            print('Wind Speed:', format(wind_speed, '.1f'), 'MPH')
            print('Report:', report.title())
            
            print('High:', format(temp_max, '.0f'), 'F')
            print('Low:', format(temp_min, '.0f'), 'F')
            print('Probabilty of Precipitation: ' + str(format(daily_precip_percent, '.0f'))  + '%')
            '''    
        else:
            # Call function to display HTTP error
            display_error('HTTP')

    # Open template file
    template = Image.open(os.path.join(picdir, 'template.bmp'))
    # Initialize the drawing context with template as background
    draw = ImageDraw.Draw(template)
    
    draw.rectangle((0, 0, 295, 67), fill=white)
    
    # Draw top left box
    ## Open icon file
    icon_file = icon_code + '.png' 
    icon_image = Image.open(os.path.join(icondir, icon_file))
    icon_image_resize = icon_image.resize((68, 68))
    
    ### Paste the image
    template.paste(icon_image_resize, (0, 0))
    ## Place a black rectangle outline
    # draw.rectangle((15, 5, 80, 60), outline=black)
    ## Draw text
    font_report = font20_Roboto_Bold
    font_report_size = 20
    while(font_report.getsize(string_report)[0] > 120):
        font_report_size -= 2
        font_report = ImageFont.truetype(os.path.join(fontdir, 'Roboto-Bold.ttf'), font_report_size)
    draw.text((70, 12), string_report, font=font_report, fill=black)
    draw.text((70, 34), string_precip_percent, font=font20_Roboto_Bold, fill=black)
    
    # Draw top right box
    draw.text((12, 72), string_temp, font=font18_Roboto_Bold, fill=white)
    # draw.text((60, 30), string_feels_like, font=font20, fill=white)
    
    # Draw bottom left box
    # draw.text((15, 85), string_temp_max, font=font20, fill=black)
    # draw.rectangle((15, 108, 265, 110), fill=black)
    # draw.text((15, 105), string_temp_min, font=font20, fill=black)
    
    # Draw bottom middle box
    # draw.text((120, 85), string_humidity, font=font20, fill=black)
    # draw.text((120, 105), string_wind, font=font20, fill=black)
    
    # Draw bottom right box
    draw.text((295-11-font20_Roboto_Regular.getsize('UPDATED')[0], 7), 'UPDATED', font=font20_Roboto_Regular, fill=black, align='right')
    current_time = datetime.now().strftime('%H:%M')
    draw.text((295-11-font34_Roboto_Black.getsize(current_time)[0], 25), current_time, font = font34_Roboto_Black, fill=black, align='right')
    # draw.rectangle((193, 35, 288, 85), outline=black)

    draw.text((295-11-font18_Roboto_Bold.getsize(string_location)[0], 72), string_location, font = font18_Roboto_Bold, fill=white, align='right')

    ## Add a reminder to take out trash on Mon and Thurs
    # weekday = datetime.today().weekday()
    # if weekday == 0 or weekday == 3:
        # draw.rectangle((345, 13, 705, 55), fill =black)
        # draw.text((355, 15), 'TAKE OUT TRASH TODAY!', font=font18, fill=white)
        
    # Save the image for display as PNG
    screen_output_file = os.path.join(picdir, 'screen_output.png')
    template.save(screen_output_file)
    # Close the template file
    template.close()
    
    # Refresh clear screen to avoid burn-in at 3:00 AM
    # if datetime.now().strftime('%H') == '03':
    	# print('Clearning screen to avoid burn-in.')
    	# epd.Clear()
    
    # Write to screen
    # write_to_screen(screen_output_file, 600)
