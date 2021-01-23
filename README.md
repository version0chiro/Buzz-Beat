
<!-- PROJECT LOGO -->
<br />
<p align="center">
  <a href="https://github.com/version0chiro/Contactless_stethoscope">
    <img src="images/logo.png" alt="Logo" width="80" height="80">
  </a>

  <h3 align="center">Buzz-Beat</h3>

  <p align="center">
    A contactless instrument for alerting doctors!
    <br />
    <a href="https://github.com/version0chiro/Contactless_stethoscope"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="https://github.com/version0chiro/Contactless_stethoscope">View Demo</a>
    ·
    <a href="https://github.com/version0chiro/Contactless_stethoscope/issues">Report Bug</a>
    ·
    <a href="https://github.com/version0chiro/Contactless_stethoscope/issues">Request Feature</a>
  </p>
</p>



<!-- TABLE OF CONTENTS -->
<details open="open">
  <summary><h2 style="display: inline-block">Table of Contents</h2></summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgements">Acknowledgements</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project
The Project is divided into four different parts and sectors and they have been listed below:
<ol>
<li><a href="https://github.com/version0chiro/Contactless_stethoscope/android_code/Neosensespo2draft">Android Application</a></li>
<li><a href="https://github.com/version0chiro/Contactless_stethoscope/web_server_code/">Web-Server</a></li>
<li><a href="https://github.com/version0chiro/Contactless_stethoscope/nodeMcu_Code/FullTestWithPost_22_1">NodeMCU-Sensors</a></li>
<li><a href="https://github.com/version0chiro/Contactless_stethoscope/edgeImpulse">Machine Learning with Edge Impulse</a></li>
</ol>
[![Product Name Screen Shot][product-screenshot]](https://example.com)


The project is aimed towards the doctors that want to get vital health related data from the paitents without having to be in contact or in a range. With the working of buzz-beat, we have made a wireless all over the globe execution for capturing and responding to sensor data for the following:
<ol>
<li>SPO2</li>
<li>Heart-Rate</li>
<li>Heart-Rate Variablity</li>
<li>Body-Temperature</li>
</ol>
<!-- image of buzz here -->
To access this data we have used the neosensory buzz! A ground shaking invention done by the people at neosensory! To find more information on the buzz be sure to click here => <a href="https://neosensory.com/product/buzz/"> Neosensory Buzz!</a>

But in general contrast, the neosensory buzz is used to convert sound data in vibration for deaf people who are unable to process these sound waves. The vibrations are given using for distinct motors attached on the band that helps the person identify different vibration patters. We wanted to go beyond this, by making something that is not just a replacement for a sense, but more like an added sense!
<!-- Image of the device here -->

Meet buzz-beat, a wireless anywhere in the world cloud based data access point for all the vital signs listen above, the hardware part of the project has been made using the following components:
<ol>
<li>NeoSensory buzz</li>
<li>Node MCU</li>
<li>Maxim Integrated MAX30100</li>
<li>MLX90614</li>
<li>Body-3.7V lipo battery</li>
</ol>

The values captured by the sensors on the device end had to be mapped onto the motors on the neosensory buzz to give tbe values for the people incharge of the paitents. This value can be sent using the the two SDKs provided by the neosensory buzz dev team,
<ul>
<li>Android SDK</li>
<li>Arduino SDK</li>
</ul>

To expand the project into a worldwide access working we choose the Android SDK and built an android app from grounds up using the example provided by here for the buzz-bluetooth library
<a href="https://github.com/neosensory/neosensory-sdk-for-android-java" >HERE</a>

<!-- Json themed image of data -->
The data on the devices was JSONified on device itself and was sent as a post request to our custom webserver made with Node.js and express, hosted on heroku

This server assignes the JSON recieved into a local JSON and then extract raw data for the Heart-Rate aswell as the Temperature, this raw data than undergoes into the WAassemply model made using edgeImpulse for Node.js

<!-- Edge Impulse pic -->

The model was trained with 2 dense layers and data was processed using dsp processing, the processing done was to insure that all axises are properly being fed into the model, rather than conventional ways of thresholding this values, using edgeImpulse we were able to make it dependent on a pattern instead.

Thus the value was not gonna be same for everyone but any kind of movement which was out of the ordinary was reported back as high temperature or high hear rate!

This value is also added to the JSON on the node server and sent to the final stage of our project- The BuzzBeat APP!

<!-- Android pics here -->


`version0chiro`, `Contactless_stethoscope`, `twitter_handle`, `email`, `project_title`, `project_description`


### Built With

* []()
* []()
* []()



<!-- GETTING STARTED -->
## Getting Started

To get a local copy up and running follow these simple steps.

### Prerequisites

This is an example of how to list things you need to use the software and how to install them.
* npm
  ```sh
  npm install npm@latest -g
  ```

### Installation

1. Clone the repo
   ```sh
   git clone https://github.com/version0chiro/Contactless_stethoscope.git
   ```
2. Install NPM packages
   ```sh
   npm install
   ```



<!-- USAGE EXAMPLES -->
## Usage

Use this space to show useful examples of how a project can be used. Additional screenshots, code examples and demos work well in this space. You may also link to more resources.

_For more examples, please refer to the [Documentation](https://example.com)_



<!-- ROADMAP -->
## Roadmap

See the [open issues](https://github.com/version0chiro/Contactless_stethoscope/issues) for a list of proposed features (and known issues).



<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to be learn, inspire, and create. Any contributions you make are **greatly appreciated**.

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request



<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE` for more information.



<!-- CONTACT -->
## Contact

Your Name - [@twitter_handle](https://twitter.com/twitter_handle) - email

Project Link: [https://github.com/version0chiro/Contactless_stethoscope](https://github.com/version0chiro/Contactless_stethoscope)



<!-- ACKNOWLEDGEMENTS -->
## Acknowledgements

* []()
* []()
* []()




