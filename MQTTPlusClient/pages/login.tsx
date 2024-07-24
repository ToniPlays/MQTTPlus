import React from 'react'
import {
    Grid,
    CircularProgress,
    Typography,
    Button,
    Tabs,
    Tab,
    TextField,
    Fade,
  } from "@mui/material"

interface Props
{

}

function Login(props: Props) {

  return (
    <Grid container className='h-100 w-100 flex justify-center align-center absolute'>
        <div className='w-60 h-100 flex flex-column justify-center align-center md:w-50 sm:display-nonde'>
            <Typography className='color-white, font-medium md:text-7xl text-4xl'>MQTTPlus</Typography>
        </div>
    </Grid>
  )
}

export default Login
